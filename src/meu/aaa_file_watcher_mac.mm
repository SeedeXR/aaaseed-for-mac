// src/meu/aaa_file_watcher_mac.mm
//
// c149-A v3 Feature 3 : FSEvents hot-reload watcher. Impl file ; see
// header for design + hermetic doctrine notes.
//
// Implementation strategy :
//   - FSEventStreamCreate on the PARENT directory of the watched file.
//     (FSEvents fires on directories, not individual files. We filter
//     by basename in the callback.)
//   - Schedule on the main runloop's kCFRunLoopDefaultMode so the
//     callback dispatches in the same thread that drives Runner::reload.
//   - 100ms debounce : track the last-fire steady_clock time, drop any
//     callback firing within the window. macOS FSEvents already
//     coalesces by latency (we pass 0.05s), but text editors that
//     write+rename can still produce a double-event ; debounce covers
//     that without losing genuine multi-second-apart edits.
//
// The FSEvents callback runs as a plain C function (FSEventStream
// callback ABI) ; the userdata `clientCallBackInfo` holds an opaque
// pointer to the FileWatcherImpl which routes back to the C++ state.

#import "aaa_file_watcher_mac.h"

#import <CoreServices/CoreServices.h>
#import <Foundation/Foundation.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace aaa
{
namespace meu
{

namespace
{
    //  Debounce window : suppress callbacks fired within this many ms
    //  of the previous fire. 100ms covers the typical editor save
    //  sequence (write + rename + mtime touch) without dropping
    //  human-scale edits.
    constexpr int kDebounceMs = 100;

    //  FSEvents latency : how long the kernel may coalesce events
    //  before delivering. 50ms gives the kernel half the debounce
    //  budget to fold together a multi-syscall save.
    constexpr CFAbsoluteTime kFsEventLatency = 0.05;

    bool file_watch_disabled_via_env()
    {
        char const* v = std::getenv( "AAA_DISABLE_FILE_WATCH" );
        return v != nullptr && v[ 0 ] == '1' && v[ 1 ] == '\0';
    }
}   //  anonymous

class FileWatcherImpl
{
public:
    FileWatcherImpl() = default;

    ~FileWatcherImpl()
    {
        unwatch();
    }

    bool watch( std::string const& path, FileWatcher::OnChange callback )
    {
        if( path.empty() ) return false;
        if( _stream != nullptr ) return false;   //  already watching

        if( file_watch_disabled_via_env() )
        {
            //  Honor opt-out : keep diagnostic state consistent so
            //  is_watching() reflects the caller's expectation, but
            //  don't actually create an FSEventStream.
            _watched_path = path;
            _watched_basename.clear();
            _callback = std::move( callback );
            _disabled = true;
            return true;
        }

        std::filesystem::path const fs_path( path );
        std::filesystem::path const parent_dir = fs_path.parent_path();
        if( parent_dir.empty() ) return false;

        std::error_code ec;
        if( !std::filesystem::is_directory( parent_dir, ec ) ) return false;

        _watched_path     = path;
        _watched_basename = fs_path.filename().string();
        _callback         = std::move( callback );
        _last_fire        = std::chrono::steady_clock::time_point::min();

        //  Build the CFArray of paths to watch (just the parent dir).
        //  -fno-objc-arc : a plain cast suffices ; no ownership transfer
        //  semantics. We retain the array so CFRelease below is symmetric.
        NSString* dir_ns = [NSString stringWithUTF8String:parent_dir.string().c_str()];
        if( dir_ns == nil )
        {
            _watched_path.clear();
            _watched_basename.clear();
            return false;
        }
        CFArrayRef paths = (CFArrayRef) CFArrayCreate(
            kCFAllocatorDefault,
            (CFTypeRef[]){ (CFTypeRef) dir_ns },
            1,
            &kCFTypeArrayCallBacks );

        FSEventStreamContext ctx;
        ctx.version         = 0;
        ctx.info            = this;
        ctx.retain          = nullptr;
        ctx.release         = nullptr;
        ctx.copyDescription = nullptr;

        _stream = FSEventStreamCreate(
            kCFAllocatorDefault,
            &FileWatcherImpl::fs_events_callback,
            &ctx,
            paths,
            kFSEventStreamEventIdSinceNow,
            kFsEventLatency,
            kFSEventStreamCreateFlagFileEvents
                | kFSEventStreamCreateFlagNoDefer );

        CFRelease( paths );

        if( _stream == nullptr )
        {
            _watched_path.clear();
            _watched_basename.clear();
            return false;
        }

        //  Schedule on the main runloop so the callback dispatches on
        //  the same thread that drives Runner::reload (called from the
        //  MTKView delegate's drawInMTKView). When there's no main
        //  runloop active (headless unit test), the stream still
        //  registers but no events flow ; tests can opt out via the
        //  env var instead.
        //  c149-A NOTE : the runloop API is marked deprecated in macOS
        //  13+ in favor of FSEventStreamSetDispatchQueue, but the
        //  dispatch-queue path requires more infrastructure (target
        //  queue lifecycle) and the runloop API still functions
        //  correctly. We suppress the deprecation warning to keep the
        //  build clean ; migration to dispatch queue is a future polish.
        CFRunLoopRef rl = CFRunLoopGetMain();
        if( rl == nullptr ) rl = CFRunLoopGetCurrent();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        FSEventStreamScheduleWithRunLoop( _stream, rl, kCFRunLoopDefaultMode );
        bool const started = FSEventStreamStart( _stream );
        if( !started )
        {
            FSEventStreamUnscheduleFromRunLoop( _stream, rl, kCFRunLoopDefaultMode );
        }
#pragma clang diagnostic pop
        if( !started )
        {
            FSEventStreamInvalidate( _stream );
            FSEventStreamRelease( _stream );
            _stream = nullptr;
            _watched_path.clear();
            _watched_basename.clear();
            return false;
        }
        return true;
    }

    void unwatch()
    {
        if( _stream != nullptr )
        {
            FSEventStreamStop( _stream );
            CFRunLoopRef rl = CFRunLoopGetMain();
            if( rl == nullptr ) rl = CFRunLoopGetCurrent();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            FSEventStreamUnscheduleFromRunLoop( _stream, rl, kCFRunLoopDefaultMode );
#pragma clang diagnostic pop
            FSEventStreamInvalidate( _stream );
            FSEventStreamRelease( _stream );
            _stream = nullptr;
        }
        _watched_path.clear();
        _watched_basename.clear();
        _callback = nullptr;
        _disabled = false;
    }

    bool        is_watching()  const { return !_watched_path.empty(); }
    std::string watched_path() const { return _watched_path; }

private:
    static void fs_events_callback(
        ConstFSEventStreamRef       /*streamRef*/,
        void*                       clientCallBackInfo,
        size_t                      numEvents,
        void*                       eventPaths,
        FSEventStreamEventFlags const eventFlags[],
        FSEventStreamEventId const  /*eventIds*/[] )
    {
        FileWatcherImpl* self = static_cast< FileWatcherImpl* >( clientCallBackInfo );
        if( self == nullptr || self->_callback == nullptr ) return;
        if( self->_watched_basename.empty() ) return;

        char const** paths = static_cast< char const** >( eventPaths );
        bool match = false;
        for( size_t i = 0; i < numEvents; ++i )
        {
            //  Filter by basename. paths[i] is the changed file's
            //  full POSIX path inside the watched parent dir.
            std::filesystem::path const p( paths[ i ] );
            if( p.filename().string() == self->_watched_basename )
            {
                match = true;
                (void) eventFlags;   //  flags only used for diagnostics
                break;
            }
        }
        if( !match ) return;

        //  Debounce.
        auto const now = std::chrono::steady_clock::now();
        auto const elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(
            now - self->_last_fire ).count();
        if( self->_last_fire != std::chrono::steady_clock::time_point::min()
            && elapsed < kDebounceMs )
        {
            return;
        }
        self->_last_fire = now;

        self->_callback();
    }

    FSEventStreamRef                                _stream = nullptr;
    std::string                                      _watched_path;
    std::string                                      _watched_basename;
    FileWatcher::OnChange                            _callback;
    std::chrono::steady_clock::time_point            _last_fire =
        std::chrono::steady_clock::time_point::min();
    bool                                             _disabled = false;
};

//  -------- Public facade -----------------------------------------------

FileWatcher::FileWatcher()
    : _impl( new FileWatcherImpl() )
{}

FileWatcher::~FileWatcher()
{
    delete _impl;
    _impl = nullptr;
}

bool FileWatcher::watch( std::string const& path, OnChange callback )
{
    return _impl ? _impl->watch( path, std::move( callback ) ) : false;
}

void FileWatcher::unwatch()
{
    if( _impl ) _impl->unwatch();
}

bool FileWatcher::is_watching() const
{
    return _impl ? _impl->is_watching() : false;
}

std::string FileWatcher::watched_path() const
{
    return _impl ? _impl->watched_path() : std::string();
}

}   //  namespace meu
}   //  namespace aaa
