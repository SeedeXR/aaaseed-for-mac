// src/audio/beat_detector.h
//
// second_todo.md S1 (c153) : native macOS AUDIO sub-library -- beat /
// onset detector core.
//
// Scope :
//   A REAL energy-based onset / beat detector. This is deliberately NOT
//   the 12-LOC upstream stub : it maintains a sliding history of per-block
//   energy, flags a beat when the instantaneous energy exceeds a running
//   average by a configurable threshold, applies a refractory period to
//   suppress double-triggers, and derives a tempo estimate (BPM) from the
//   median inter-onset interval.
//
// Doctrine (feedback_hermetic_mac_sublibs.md) :
//   - This class is PURE : std:: only. NO AVFoundation, NO CoreAudio, NO
//     Foundation, NO engine link, NO o_str, NO aaa_mem, NO vendor includes.
//   - Fully deterministic : fed blocks of float samples + sample_rate via
//     process_block(const float*, size_t, double). This makes it unit-
//     testable WITHOUT any audio hardware (see
//     tests/native/audio_beat_detector_test.cpp).
//   - The live audio path (Input tap in audio_mac.{h,mm}) feeds captured
//     PCM blocks into process_block() ; the detector itself never touches
//     hardware.

#pragma once

#include <cstddef>
#include <deque>
#include <vector>

namespace aaa
{
namespace audio
{

//	Tunable parameters for the onset detector. Defaults are chosen for
//	music-rate detection (kicks / snares) on ~1024-frame blocks at 44.1 kHz.
struct BeatDetectorConfig
{
    //	How many recent energy values form the running-average window. A
    //	beat fires when the current block energy exceeds the window mean
    //	scaled by sensitivity.
    std::size_t history_size = 43;

    //	Multiplier on the running mean energy that the current block must
    //	exceed to count as an onset. 1.0 = at-mean ; >1 = louder than mean.
    //	~1.3 catches clear transients without firing on steady tone.
    double sensitivity = 1.3;

    //	Absolute energy floor : blocks below this (e.g. silence / hiss) can
    //	never be onsets, regardless of the relative test. Guards against the
    //	mean collapsing to ~0 during silence and then any tiny tick firing.
    double noise_floor = 1.0e-4;

    //	Minimum seconds between two accepted onsets. Suppresses the multi-
    //	block "ring" of a single transient. 0.12 s -> max ~500 BPM, well
    //	above any musical tempo we care about.
    double refractory_seconds = 0.12;
};

//	Energy-based onset / beat detector. Single-threaded ; feed it
//	consecutive blocks of mono (or pre-mixed) float PCM.
class BeatDetector
{
public:
    BeatDetector();
    explicit BeatDetector( BeatDetectorConfig const & cfg );

    //	Feed one block of samples. `samples` may be null only if
    //	`frame_count` is 0. `sample_rate` must be > 0 ; a non-positive rate
    //	is treated as a no-op (block ignored). Computes block RMS energy,
    //	compares against the running average, and -- subject to the
    //	refractory period -- may flag a beat for THIS block. Updates the
    //	energy history and onset-time bookkeeping.
    void process_block( float const * samples, std::size_t frame_count,
                        double sample_rate );

    //	True if the most recent process_block() call flagged a beat. Reset
    //	to false at the start of every process_block().
    bool beat_detected_in_last_block() const { return beat_in_last_block_; }

    //	Estimated tempo in beats-per-minute derived from the median of the
    //	recent inter-onset intervals. Returns 0.0 until at least two onsets
    //	have been observed (no interval to measure yet).
    double estimate_bpm() const;

    //	Total number of onsets accepted since construction / last reset().
    std::size_t beat_count() const { return beat_count_; }

    //	Clear all state : energy history, onset times, counters, the
    //	last-block flag and the elapsed-time accumulator. Config is kept.
    void reset();

    BeatDetectorConfig const & config() const { return cfg_; }

private:
    BeatDetectorConfig cfg_;

    //	Sliding window of recent block energies (RMS^2). Front = oldest.
    std::deque<double> energy_history_;
    double             energy_sum_ = 0.0;   //	cached sum of energy_history_

    //	Monotonic playback time accumulated from frame_count / sample_rate.
    double elapsed_seconds_ = 0.0;

    //	Absolute time of the most recently accepted onset, and the inter-
    //	onset intervals (seconds) used for the BPM estimate.
    double              last_onset_time_   = -1.0e9;
    bool                have_onset_        = false;
    std::vector<double> inter_onset_gaps_;

    bool        beat_in_last_block_ = false;
    std::size_t beat_count_         = 0;
};

}   //	namespace audio
}   //	namespace aaa
