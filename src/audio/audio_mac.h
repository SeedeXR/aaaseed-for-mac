// src/audio/audio_mac.h
//
// second_todo.md S1 (c153) : native macOS AUDIO sub-library.
//
// Scope :
//   The Mac-side audio surface for the AAASeed port. Three pieces :
//
//     1. Device enumeration -- output / input device counts and names,
//        via the CoreAudio AudioObject property API
//        (AudioObjectGetPropertyDataSize / AudioObjectGetPropertyData on
//        kAudioObjectSystemObject with kAudioHardwarePropertyDevices, then
//        kAudioObjectPropertyName per device ; input-vs-output is
//        classified by querying kAudioDevicePropertyStreams on the input /
//        output scope). All getters are crash-safe with zero devices.
//
//     2. class Player -- file playback. AVAudioEngine + AVAudioPlayerNode +
//        AVAudioFile. Mirrors the upstream c_sound_player_coreaudio surface
//        (volume / pan / position / rate("frequency")). Position is tracked
//        from the player's sample time relative to its sample rate.
//
//     3. class Input -- live capture. AVAudioEngine inputNode
//        installTapOnBus:bufferSize:format:block: , delivering de-
//        interleaved channel-0 float samples to a std::function callback.
//        start() with no usable input device returns false (no crash).
//
// Doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + AVFoundation / AVFAudio + CoreAudio +
//     AudioToolbox + Foundation only. No engine link, no o_str, no
//     aaa_mem, no vendor includes.
//   - Built with -fno-objc-arc : the .mm does MANUAL retain / release on
//     every ObjC object it owns. Callers of this header never see ObjC.
//   - This header is C++-clean : no ObjC types cross the boundary. The
//     impl pointers are hidden behind opaque void* so plain .cpp TUs can
//     include it.
//
// The PURE, hardware-free onset detector lives in beat_detector.{h,mm} ;
// the Input callback is the intended feeder for BeatDetector::process_block.

#pragma once

#include <cstddef>
#include <functional>
#include <string>

namespace aaa
{
namespace audio
{

//	==================================================================
//	Device enumeration (CoreAudio AudioObject property API)
//	==================================================================

//	Number of hardware devices that expose at least one OUTPUT stream.
//	Returns 0 (never throws / crashes) when no devices are present.
std::size_t get_output_device_count();

//	Number of hardware devices that expose at least one INPUT stream.
std::size_t get_input_device_count();

//	Human-readable name (UTF-8) of the index-th OUTPUT device, in the same
//	order get_output_device_count() counts. Out-of-range index returns "".
std::string get_output_device_name( std::size_t index );

//	Human-readable name (UTF-8) of the index-th INPUT device. Out-of-range
//	index returns "".
std::string get_input_device_name( std::size_t index );

//	==================================================================
//	class Player -- AVAudioEngine + AVAudioPlayerNode + AVAudioFile
//	==================================================================
class Player
{
public:
    Player();
    ~Player();

    Player( Player const & )             = delete;
    Player & operator=( Player const & ) = delete;

    //	Load an audio file (any format AVAudioFile reads : wav / aiff /
    //	caf / m4a / mp3 ...). Rebuilds the engine graph around the new
    //	file. Returns false if the path does not exist / cannot be opened,
    //	or if the audio engine fails to start. Safe to call repeatedly.
    bool open( std::string const & path );

    //	Start (or resume) playback from the current position. No-op if no
    //	file is open. The file is scheduled exactly once per open() ; play()
    //	after stop() re-schedules from the start.
    void play();

    //	Pause playback, retaining position. play() resumes.
    void pause();

    //	Stop playback and rewind to the start (position 0).
    void stop();

    //	Output gain, clamped to [0, 1]. Maps to the player node volume.
    void  set_volume( float volume );
    float get_volume() const;

    //	Stereo pan, clamped to [-1, 1] (left .. right). Mirrors the upstream
    //	pan control ; optional per the spec but cheap to expose.
    void  set_pan( float pan );
    float get_pan() const;

    //	Playback rate / "frequency" multiplier (1.0 = normal). Implemented
    //	via an AVAudioUnitVarispeed node, so pitch tracks rate. Clamped to a
    //	sane positive range.
    void  set_rate( float rate );
    float get_rate() const;

    //	Current playback position in seconds, derived from the player
    //	node's sample time. Returns 0 when not playing / no file.
    double position_seconds() const;

    //	Total file duration in seconds (length / sample-rate). 0 if no file.
    double duration_seconds() const;

    //	True while the player node is actively rendering.
    bool is_playing() const;

private:
    struct Impl;
    Impl * impl_;   //	owns all ObjC objects ; manual retain/release
};

//	==================================================================
//	class Input -- AVAudioEngine inputNode installTapOnBus
//	==================================================================
class Input
{
public:
    //	Called from the audio render thread with de-interleaved channel-0
    //	float PCM. `samples` is valid only for the duration of the call.
    //	`frame_count` is the number of float samples ; `sample_rate` is the
    //	tap format's sample rate. Keep the callback fast and lock-free.
    using SampleCallback =
        std::function<void( float const * samples, std::size_t frame_count,
                            double sample_rate )>;

    Input();
    ~Input();

    Input( Input const & )             = delete;
    Input & operator=( Input const & ) = delete;

    //	Install a tap on the default input device and start the engine.
    //	Returns false (and installs nothing) if there is no usable input
    //	device or the engine fails to start -- never crashes on a machine
    //	with no microphone. Calling start() twice without stop() first
    //	returns false.
    bool start( SampleCallback callback );

    //	Remove the tap and stop the engine. Safe to call when not started.
    void stop();

    //	True between a successful start() and the next stop().
    bool is_running() const;

private:
    struct Impl;
    Impl * impl_;   //	owns all ObjC objects ; manual retain/release
};

}   //	namespace audio
}   //	namespace aaa
