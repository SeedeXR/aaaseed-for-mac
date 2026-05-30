// src/audio/audio_mac.mm
//
// second_todo.md S1 (c153) : native macOS AUDIO sub-library implementation.
// Device enumeration (CoreAudio AudioObject property API) + Player
// (AVAudioEngine + AVAudioPlayerNode + AVAudioUnitVarispeed + AVAudioFile) +
// Input (AVAudioEngine inputNode tap). See audio_mac.h for the API + doctrine.
// Pure ObjC++, manual reference counting (-fno-objc-arc).

#import "src/audio/audio_mac.h"

#import <AVFoundation/AVFoundation.h>
#import <CoreAudio/CoreAudio.h>

#include <vector>

namespace aaa
{
namespace audio
{

//	==================================================================
//	Device enumeration -- CoreAudio AudioObject property API
//	==================================================================
namespace
{

//	Pull the system device-ID list. Returns an empty vector on any error
//	(crash-safe with zero devices).
std::vector<AudioDeviceID> all_device_ids()
{
    std::vector<AudioDeviceID> ids;

    AudioObjectPropertyAddress addr = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain
    };

    UInt32 size = 0;
    OSStatus st = AudioObjectGetPropertyDataSize(
        kAudioObjectSystemObject, &addr, 0, nullptr, &size );
    if( st != noErr || size == 0 )
        return ids;

    std::size_t const count = size / sizeof( AudioDeviceID );
    ids.resize( count );
    st = AudioObjectGetPropertyData(
        kAudioObjectSystemObject, &addr, 0, nullptr, &size, ids.data() );
    if( st != noErr )
        ids.clear();
    return ids;
}

//	True if `dev` exposes >=1 stream on the given scope (input or output).
bool device_has_streams( AudioDeviceID dev, AudioObjectPropertyScope scope )
{
    AudioObjectPropertyAddress addr = {
        kAudioDevicePropertyStreams,
        scope,
        kAudioObjectPropertyElementMain
    };
    UInt32 size = 0;
    OSStatus const st = AudioObjectGetPropertyDataSize(
        dev, &addr, 0, nullptr, &size );
    if( st != noErr )
        return false;
    return ( size / sizeof( AudioStreamID ) ) > 0;
}

std::string device_name( AudioDeviceID dev )
{
    AudioObjectPropertyAddress addr = {
        kAudioObjectPropertyName,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain
    };
    CFStringRef name = nullptr;
    UInt32 size = sizeof( name );
    OSStatus const st = AudioObjectGetPropertyData(
        dev, &addr, 0, nullptr, &size, &name );
    if( st != noErr || name == nullptr )
        return std::string();

    std::string out;
    char buf[ 512 ];
    if( CFStringGetCString( name, buf, sizeof( buf ), kCFStringEncodingUTF8 ) )
        out = buf;
    CFRelease( name );
    return out;
}

//	Collect the device IDs that expose streams on `scope`, preserving the
//	system order so count and name getters agree on indexing.
std::vector<AudioDeviceID> devices_for_scope( AudioObjectPropertyScope scope )
{
    std::vector<AudioDeviceID> out;
    for( AudioDeviceID dev : all_device_ids() )
        if( device_has_streams( dev, scope ) )
            out.push_back( dev );
    return out;
}

}   //	anonymous namespace

std::size_t get_output_device_count()
{
    return devices_for_scope( kAudioObjectPropertyScopeOutput ).size();
}

std::size_t get_input_device_count()
{
    return devices_for_scope( kAudioObjectPropertyScopeInput ).size();
}

std::string get_output_device_name( std::size_t index )
{
    auto const devs = devices_for_scope( kAudioObjectPropertyScopeOutput );
    if( index >= devs.size() )
        return std::string();
    return device_name( devs[ index ] );
}

std::string get_input_device_name( std::size_t index )
{
    auto const devs = devices_for_scope( kAudioObjectPropertyScopeInput );
    if( index >= devs.size() )
        return std::string();
    return device_name( devs[ index ] );
}

//	==================================================================
//	class Player
//	==================================================================
struct Player::Impl
{
    AVAudioEngine *        engine    = nil;
    AVAudioPlayerNode *    node      = nil;
    AVAudioUnitVarispeed * varispeed = nil;
    AVAudioFile *          file      = nil;

    double sample_rate = 0.0;
    double duration    = 0.0;
    float  volume      = 1.0f;
    float  pan         = 0.0f;
    float  rate        = 1.0f;
    bool   playing     = false;

    void teardown()
    {
        if( node != nil && engine != nil )
            [node stop];
        if( engine != nil )
            [engine stop];
        [file release];      file      = nil;
        [node release];      node      = nil;
        [varispeed release]; varispeed = nil;
        [engine release];    engine    = nil;
        sample_rate = 0.0;
        duration    = 0.0;
        playing     = false;
    }
};

Player::Player() : impl_( new Impl() ) {}

Player::~Player()
{
    impl_->teardown();
    delete impl_;
}

bool Player::open( std::string const & path )
{
    impl_->teardown();

    @autoreleasepool
    {
        NSString * ns = [[[NSString alloc] initWithBytes:path.data()
                                                  length:(NSUInteger)path.size()
                                                encoding:NSUTF8StringEncoding] autorelease];
        if( ns == nil )
            return false;
        NSURL * url = [NSURL fileURLWithPath:ns];
        if( url == nil )
            return false;

        NSError *     err  = nil;
        AVAudioFile * file = [[AVAudioFile alloc] initForReading:url error:&err];
        if( file == nil || err != nil )
        {
            [file release];
            return false;
        }

        AVAudioEngine *        engine    = [[AVAudioEngine alloc] init];
        AVAudioPlayerNode *    node      = [[AVAudioPlayerNode alloc] init];
        AVAudioUnitVarispeed * varispeed = [[AVAudioUnitVarispeed alloc] init];

        [engine attachNode:node];
        [engine attachNode:varispeed];

        AVAudioFormat * fmt = file.processingFormat;
        [engine connect:node      to:varispeed            format:fmt];
        [engine connect:varispeed to:engine.mainMixerNode format:fmt];

        NSError * start_err = nil;
        if( ![engine startAndReturnError:&start_err] || start_err != nil )
        {
            [file release];
            [node release];
            [varispeed release];
            [engine release];
            return false;
        }

        [node scheduleFile:file atTime:nil completionHandler:nil];

        impl_->engine      = engine;       //	transfer ownership (+1 each)
        impl_->node        = node;
        impl_->varispeed   = varispeed;
        impl_->file        = file;
        impl_->sample_rate = fmt.sampleRate;
        impl_->duration    = ( fmt.sampleRate > 0.0 )
            ? (double) file.length / fmt.sampleRate
            : 0.0;

        node.volume = impl_->volume;
        node.pan    = impl_->pan;
        varispeed.rate = impl_->rate;
        return true;
    }
}

void Player::play()
{
    if( impl_->node == nil )
        return;
    [impl_->node play];
    impl_->playing = true;
}

void Player::pause()
{
    if( impl_->node == nil )
        return;
    [impl_->node pause];
    impl_->playing = false;
}

void Player::stop()
{
    if( impl_->node == nil )
        return;
    [impl_->node stop];
    impl_->playing = false;
    //	Re-schedule from the start so a subsequent play() restarts cleanly.
    if( impl_->file != nil )
        [impl_->node scheduleFile:impl_->file atTime:nil completionHandler:nil];
}

void Player::set_volume( float volume )
{
    if( volume < 0.0f ) volume = 0.0f;
    if( volume > 1.0f ) volume = 1.0f;
    impl_->volume = volume;
    if( impl_->node != nil )
        impl_->node.volume = volume;
}

float Player::get_volume() const { return impl_->volume; }

void Player::set_pan( float pan )
{
    if( pan < -1.0f ) pan = -1.0f;
    if( pan >  1.0f ) pan =  1.0f;
    impl_->pan = pan;
    if( impl_->node != nil )
        impl_->node.pan = pan;
}

float Player::get_pan() const { return impl_->pan; }

void Player::set_rate( float rate )
{
    if( rate < 0.25f ) rate = 0.25f;
    if( rate > 4.0f )  rate = 4.0f;
    impl_->rate = rate;
    if( impl_->varispeed != nil )
        impl_->varispeed.rate = rate;
}

float Player::get_rate() const { return impl_->rate; }

double Player::position_seconds() const
{
    if( impl_->node == nil || impl_->sample_rate <= 0.0 )
        return 0.0;
    AVAudioTime * last = [impl_->node lastRenderTime];
    if( last == nil || !last.sampleTimeValid )
        return 0.0;
    AVAudioTime * pt = [impl_->node playerTimeForNodeTime:last];
    if( pt == nil || !pt.sampleTimeValid )
        return 0.0;
    return (double) pt.sampleTime / impl_->sample_rate;
}

double Player::duration_seconds() const { return impl_->duration; }

bool Player::is_playing() const
{
    if( impl_->node == nil )
        return false;
    return impl_->node.isPlaying ? true : false;
}

//	==================================================================
//	class Input
//	==================================================================
struct Input::Impl
{
    AVAudioEngine * engine   = nil;
    SampleCallback  callback;
    bool            running  = false;

    void teardown()
    {
        if( engine != nil )
        {
            [[engine inputNode] removeTapOnBus:0];
            [engine stop];
            [engine release];
            engine = nil;
        }
        running = false;
    }
};

Input::Input() : impl_( new Impl() ) {}

Input::~Input()
{
    impl_->teardown();
    delete impl_;
}

bool Input::start( SampleCallback callback )
{
    if( impl_->running )
        return false;

    @autoreleasepool
    {
        AVAudioEngine *   engine = [[AVAudioEngine alloc] init];
        AVAudioInputNode * in    = [engine inputNode];
        AVAudioFormat *    fmt   = [in inputFormatForBus:0];

        //	No usable input device : sampleRate / channelCount come back 0.
        if( fmt == nil || fmt.sampleRate <= 0.0 || fmt.channelCount == 0 )
        {
            [engine release];
            return false;
        }

        impl_->callback = callback;
        Impl * impl = impl_;

        [in installTapOnBus:0
                 bufferSize:1024
                     format:fmt
                      block:^( AVAudioPCMBuffer * buf, AVAudioTime * /*when*/ )
        {
            if( buf == nil || !impl->callback )
                return;
            float const * const * ch = buf.floatChannelData;
            if( ch == nullptr )
                return;
            impl->callback( ch[ 0 ],
                            (std::size_t) buf.frameLength,
                            buf.format.sampleRate );
        }];

        NSError * err = nil;
        if( ![engine startAndReturnError:&err] || err != nil )
        {
            [in removeTapOnBus:0];
            [engine release];
            impl_->callback = SampleCallback();
            return false;
        }

        impl_->engine  = engine;   //	transfer ownership (+1)
        impl_->running = true;
        return true;
    }
}

void Input::stop()    { impl_->teardown(); }
bool Input::is_running() const { return impl_->running; }

}   //	namespace audio
}   //	namespace aaa
