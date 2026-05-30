// tests/native/audio_beat_detector_test.cpp
//
// second_todo.md S1 (c153) : PURE unit coverage for the energy-based onset /
// beat detector. No audio hardware : a synthetic click track at a known tempo
// is fed block-by-block and the detector's beat count + estimate_bpm() are
// checked against tolerance. Also: silence yields no beats ; reset() clears.

#include <gtest/gtest.h>

#include "src/audio/beat_detector.h"

#include <cstddef>
#include <vector>

namespace
{

constexpr double      kSampleRate = 44100.0;
constexpr std::size_t kBlock      = 1024;

//	Build `beats` clicks at the given BPM : each click is a short loud burst,
//	the rest is near-silence. Returns one contiguous mono float buffer.
std::vector<float> make_click_track( double bpm, int beats )
{
    double const  period_s     = 60.0 / bpm;
    std::size_t   period_frames = (std::size_t)( period_s * kSampleRate );
    std::size_t   total         = period_frames * (std::size_t) beats;
    std::vector<float> buf( total, 0.0f );

    constexpr std::size_t burst = 128;   //	~3 ms click
    for( int b = 0; b < beats; ++b )
    {
        std::size_t start = (std::size_t) b * period_frames;
        for( std::size_t i = 0; i < burst && ( start + i ) < total; ++i )
            buf[ start + i ] = 1.0f;
    }
    return buf;
}

//	Feed a buffer through the detector in kBlock chunks ; return total beats.
std::size_t feed( aaa::audio::BeatDetector & det, std::vector<float> const & buf )
{
    std::size_t beats = 0;
    for( std::size_t off = 0; off < buf.size(); off += kBlock )
    {
        std::size_t n = buf.size() - off;
        if( n > kBlock ) n = kBlock;
        det.process_block( buf.data() + off, n, kSampleRate );
        if( det.beat_detected_in_last_block() )
            ++beats;
    }
    return beats;
}

}   //	anonymous namespace

TEST( BeatDetector, DetectsClicksNear120Bpm )
{
    aaa::audio::BeatDetector det;
    auto track = make_click_track( 120.0, 12 );
    std::size_t beats = feed( det, track );

    //	12 clicks emitted ; allow +/- 2 for block-quantization edge effects.
    EXPECT_GE( beats, (std::size_t)10 );
    EXPECT_LE( beats, (std::size_t)13 );
    EXPECT_EQ( det.beat_count(), beats );

    //	Tempo from median inter-onset interval, +/- 8 BPM tolerance.
    double bpm = det.estimate_bpm();
    EXPECT_GT( bpm, 112.0 );
    EXPECT_LT( bpm, 128.0 );
}

TEST( BeatDetector, DetectsClicksNear90Bpm )
{
    aaa::audio::BeatDetector det;
    auto track = make_click_track( 90.0, 10 );
    feed( det, track );
    double bpm = det.estimate_bpm();
    EXPECT_GT( bpm, 82.0 );
    EXPECT_LT( bpm, 98.0 );
}

TEST( BeatDetector, SilenceProducesNoBeats )
{
    aaa::audio::BeatDetector det;
    std::vector<float> silence( kBlock * 200, 0.0f );
    std::size_t beats = feed( det, silence );
    EXPECT_EQ( beats, (std::size_t)0 );
    EXPECT_EQ( det.estimate_bpm(), 0.0 );
}

TEST( BeatDetector, ResetClearsState )
{
    aaa::audio::BeatDetector det;
    feed( det, make_click_track( 120.0, 8 ) );
    ASSERT_GT( det.beat_count(), (std::size_t)0 );
    det.reset();
    EXPECT_EQ( det.beat_count(), (std::size_t)0 );
    EXPECT_EQ( det.estimate_bpm(), 0.0 );
    EXPECT_FALSE( det.beat_detected_in_last_block() );
}

TEST( BeatDetector, NonPositiveSampleRateIsNoOp )
{
    aaa::audio::BeatDetector det;
    std::vector<float> blk( kBlock, 1.0f );
    det.process_block( blk.data(), blk.size(), 0.0 );
    EXPECT_FALSE( det.beat_detected_in_last_block() );
    EXPECT_EQ( det.beat_count(), (std::size_t)0 );
}
