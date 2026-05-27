// src/meu/aaa_file_watcher_mac.h
//
// c149-A v3 Feature 3 : FSEvents hot-reload watcher. Auto-triggers
// Runner::reload() whenever the watched .lua file changes on disk.
// Completes the in-app authoring loop (Phase 4 hot_reload_button was
// click-only ; this layer adds the file-watch trigger).
//
// Hermetic Mac sub-lib per feedback_hermetic_mac_sublibs.md :
//   - Pure C++ header (no ObjC types) ; .mm impl uses CoreServices'
//     FSEventStream API + std:: primitives only.
//   - NO o_str / aaa_str.h / aaa_mem.h.
//   - std::string / std::function / <chrono> only.
//
// Lifetime contract :
//   - Construct cheap (no resources allocated until watch() called).
//   - watch(path, cb) opens an FSEventStream on the parent directory
//     of `path` ; the callback filters by exact basename match.
//   - Debounces : suppresses callbacks fired within 100ms of the most
//     recent callback (FSEvents coalesces but can still fire twice on
//     a single editor save).
//   - unwatch() invalidates + releases the stream ; safe to re-call.
//   - dtor calls unwatch() so leaks are impossible.
//
// Test override : AAA_DISABLE_FILE_WATCH env var = "1" makes watch()
// a no-op (returns true to keep test-side bookkeeping clean) so
// regression tests that don't want surprise reloads can opt out.

#pragma once

#include <functional>
#include <string>

namespace aaa
{
namespace meu
{

class FileWatcherImpl;

class FileWatcher
{
public:
    using OnChange = std::function< void() >;

    FileWatcher();
    ~FileWatcher();

    FileWatcher( FileWatcher const& )            = delete;
    FileWatcher& operator=( FileWatcher const& ) = delete;

    //  Start watching `path` (must be an existing regular file). The
    //  callback fires on the main runloop whenever the file's mtime
    //  / inode / content changes. Returns true on success, false when :
    //    - `path` is empty.
    //    - The parent directory does not exist.
    //    - FSEventStreamCreate fails.
    //    - Already watching ; call unwatch() first.
    //  AAA_DISABLE_FILE_WATCH=1 makes this a no-op-returning-true.
    bool watch( std::string const& path, OnChange callback );

    //  Stop watching. Idempotent.
    void unwatch();

    //  Diagnostics : is a watch currently active ?
    bool is_watching() const;

    //  Diagnostics : the path most recently passed to watch() (empty
    //  when no watch is active). Used by tests to validate routing.
    std::string watched_path() const;

private:
    FileWatcherImpl* _impl;   //  owned ; raw to keep header pimpl-free
};

}   //  namespace meu
}   //  namespace aaa
