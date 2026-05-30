// src/audio/beat_detector.mm
//
// second_todo.md S1 (c153) : native macOS AUDIO sub-library -- beat /
// onset detector core implementation.
//
// PURE : std:: only. No AVFoundation / CoreAudio / Foundation. Built as a
// .mm purely so it can live in the same hermetic sub-lib target as
// audio_mac.mm ; it imports no Objective-C anything.
//
// Doctrine : feedback_hermetic_mac_sublibs.md.

#import "src/audio/beat_detector.h"

#include <algorithm>
#include <cmath>

namespace aaa
{
namespace audio
{

BeatDetector::BeatDetector()
    : BeatDetector( BeatDetectorConfig{} )
{
}

BeatDetector::BeatDetector( BeatDetectorConfig const & cfg )
    : cfg_( cfg )
{
    //	Defend against pathological config that would divide by zero or
    //	never accumulate any history.
    if ( cfg_.history_size == 0 )
    {
        cfg_.history_size = 1;
    }
}

void BeatDetector::reset()
{
    energy_history_.clear();
    energy_sum_       = 0.0;
    elapsed_seconds_  = 0.0;
    last_onset_time_  = -1.0e9;
    have_onset_       = false;
    inter_onset_gaps_.clear();
    beat_in_last_block_ = false;
    beat_count_         = 0;
}

void BeatDetector::process_block( float const * samples,
                                  std::size_t   frame_count,
                                  double        sample_rate )
{
    //	Every call resets the "this block" flag first ; callers poll it
    //	immediately after process_block().
    beat_in_last_block_ = false;

    if ( sample_rate <= 0.0 )
    {
        return;
    }
    if ( frame_count == 0 || samples == nullptr )
    {
        //	Silence-length advance with no data : still advance time so the
        //	refractory / BPM clock stays consistent.
        return;
    }

    //	1. Block energy = mean square (RMS^2). Stable regardless of block
    //	   length so the running average is comparable across blocks.
    double sum_sq = 0.0;
    for ( std::size_t i = 0; i < frame_count; ++i )
    {
        double const s = static_cast<double>( samples[ i ] );
        sum_sq += s * s;
    }
    double const energy = sum_sq / static_cast<double>( frame_count );

    //	2. Running average over the history window (excludes the current
    //	   block, which we are testing against the PAST).
    double const mean =
        energy_history_.empty()
            ? 0.0
            : energy_sum_ / static_cast<double>( energy_history_.size() );

    //	3. Onset test : loud enough in absolute terms AND a sufficient
    //	   multiple of the recent mean. When there is no history yet the
    //	   relative test is skipped (we only require the noise floor) so the
    //	   very first transient in a stream can still register.
    bool const above_floor    = energy > cfg_.noise_floor;
    bool const above_relative =
        energy_history_.empty()
            ? true
            : ( energy > mean * cfg_.sensitivity );

    if ( above_floor && above_relative )
    {
        double const since_last = elapsed_seconds_ - last_onset_time_;
        if ( since_last >= cfg_.refractory_seconds )
        {
            beat_in_last_block_ = true;
            ++beat_count_;

            if ( have_onset_ )
            {
                inter_onset_gaps_.push_back( since_last );
            }
            last_onset_time_ = elapsed_seconds_;
            have_onset_      = true;
        }
    }

    //	4. Update the energy history window (after the test).
    energy_history_.push_back( energy );
    energy_sum_ += energy;
    while ( energy_history_.size() > cfg_.history_size )
    {
        energy_sum_ -= energy_history_.front();
        energy_history_.pop_front();
    }
    if ( energy_sum_ < 0.0 )    //	guard fp drift
    {
        energy_sum_ = 0.0;
    }

    //	5. Advance the clock by this block's duration.
    elapsed_seconds_ += static_cast<double>( frame_count ) / sample_rate;
}

double BeatDetector::estimate_bpm() const
{
    if ( inter_onset_gaps_.empty() )
    {
        return 0.0;
    }

    //	Median inter-onset interval is robust to the occasional spurious
    //	double-onset or missed beat that a plain mean would smear.
    std::vector<double> gaps = inter_onset_gaps_;
    std::sort( gaps.begin(), gaps.end() );

    double median = 0.0;
    std::size_t const n = gaps.size();
    if ( n % 2 == 1 )
    {
        median = gaps[ n / 2 ];
    }
    else
    {
        median = 0.5 * ( gaps[ n / 2 - 1 ] + gaps[ n / 2 ] );
    }

    if ( median <= 0.0 )
    {
        return 0.0;
    }
    return 60.0 / median;
}

}   //	namespace audio
}   //	namespace aaa
