#pragma once

#include <juce_dsp/juce_dsp.h>

/**
 * DeEsserDSP - Multi-band de-esser with per-band attenuation blend and HF limiter.
 *
 * Fixed frequency bands: 2-4kHz, 4-7kHz, 7-10kHz, 10kHz+
 * Single global threshold with per-band amount (0-100%) for surgical control.
 * HF limiter (SSL Fusion style) on content above ~6kHz.
 */
class DeEsserDSP {
public:
    static constexpr int NUM_BANDS = 4;

    /** Crossover frequencies: 2k, 4k, 7k, 10k */
    static constexpr double kCrossoverFreqs[4] = { 2000.0, 4000.0, 7000.0, 10000.0 };

    /** HF limiter crossover (6kHz) */
    static constexpr double kHFLimiterFreq = 6000.0;

    DeEsserDSP();
    ~DeEsserDSP() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();

    /** Process stereo block. Parameters passed per-sample or updated from host. */
    void process(juce::AudioBuffer<float>& buffer,
                 float thresholdDb,
                 float bandAmounts[NUM_BANDS],
                 bool hfLimiterEnabled);

private:
    // LR4 band-split filters (Butterworth 2nd-order x2)
    struct SecondOrderIIR {
        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;
        float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };

    struct LR4Filter {
        SecondOrderIIR stage1, stage2;
    };

    LR4Filter lpFiltersL[4];  // LP at 2k, 4k, 7k, 10k (left)
    LR4Filter lpFiltersR[4];  // LP at 2k, 4k, 7k, 10k (right)
    LR4Filter hfLpFilterL;    // LP at 6k for HF limiter (left)
    LR4Filter hfLpFilterR;    // LP at 6k for HF limiter (right)

    float sampleRate = 44100.0f;

    // Envelope follower per band, per channel (stereo)
    float envelopeL[NUM_BANDS] = { 0 };
    float envelopeR[NUM_BANDS] = { 0 };

    // Envelope coefficients (attack ~2ms, release ~80ms)
    static constexpr float kAttackMs = 2.0f;
    static constexpr float kReleaseMs = 80.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;

    // Fixed ratio 4:1
    static constexpr float kRatio = 4.0f;

    void updateFilterCoefficients();
    void updateEnvelopeCoefficients();
    static float processIIR(SecondOrderIIR& f, float x);
    static float processLR4LowPass(LR4Filter& f, float x);
    void processBand(int band, float inputL, float inputR,
                    float amount, float thresholdLinear,
                    float& outL, float& outR);
};
