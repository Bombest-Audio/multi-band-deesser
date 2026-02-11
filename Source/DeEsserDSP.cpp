#include "DeEsserDSP.h"
#include <cmath>

namespace {
    constexpr double kPi = 3.14159265358979323846;
}

DeEsserDSP::DeEsserDSP() {
    reset();
}

void DeEsserDSP::prepare(double sampleRateIn, int /*samplesPerBlock*/) {
    sampleRate = static_cast<float>(sampleRateIn);
    updateFilterCoefficients();
    updateEnvelopeCoefficients();
    reset();
}

void DeEsserDSP::reset() {
    for (int i = 0; i < 4; ++i) {
        lpFiltersL[i].stage1.x1 = lpFiltersL[i].stage1.x2 = lpFiltersL[i].stage1.y1 = lpFiltersL[i].stage1.y2 = 0.0f;
        lpFiltersL[i].stage2.x1 = lpFiltersL[i].stage2.x2 = lpFiltersL[i].stage2.y1 = lpFiltersL[i].stage2.y2 = 0.0f;
        lpFiltersR[i].stage1.x1 = lpFiltersR[i].stage1.x2 = lpFiltersR[i].stage1.y1 = lpFiltersR[i].stage1.y2 = 0.0f;
        lpFiltersR[i].stage2.x1 = lpFiltersR[i].stage2.x2 = lpFiltersR[i].stage2.y1 = lpFiltersR[i].stage2.y2 = 0.0f;
    }
    hfLpFilterL.stage1.x1 = hfLpFilterL.stage1.x2 = hfLpFilterL.stage1.y1 = hfLpFilterL.stage1.y2 = 0.0f;
    hfLpFilterL.stage2.x1 = hfLpFilterL.stage2.x2 = hfLpFilterL.stage2.y1 = hfLpFilterL.stage2.y2 = 0.0f;
    hfLpFilterR.stage1.x1 = hfLpFilterR.stage1.x2 = hfLpFilterR.stage1.y1 = hfLpFilterR.stage1.y2 = 0.0f;
    hfLpFilterR.stage2.x1 = hfLpFilterR.stage2.x2 = hfLpFilterR.stage2.y1 = hfLpFilterR.stage2.y2 = 0.0f;

    for (int i = 0; i < NUM_BANDS; ++i) {
        envelopeL[i] = envelopeR[i] = 0.0f;
    }
}

void DeEsserDSP::updateFilterCoefficients() {
    const float sqrt2 = std::sqrt(2.0f);

    for (int i = 0; i < 4; ++i) {
        const float fc = static_cast<float>(kCrossoverFreqs[i]);
        const float omega = 2.0f * static_cast<float>(kPi) * fc / sampleRate;
        const float k = std::tan(omega / 2.0f);
        const float k2 = k * k;
        const float norm = 1.0f / (1.0f + sqrt2 * k + k2);

        lpFiltersL[i].stage1.b0 = lpFiltersL[i].stage2.b0 = k2 * norm;
        lpFiltersL[i].stage1.b1 = lpFiltersL[i].stage2.b1 = 2.0f * k2 * norm;
        lpFiltersL[i].stage1.b2 = lpFiltersL[i].stage2.b2 = k2 * norm;
        lpFiltersL[i].stage1.a1 = lpFiltersL[i].stage2.a1 = 2.0f * (k2 - 1.0f) * norm;
        lpFiltersL[i].stage1.a2 = lpFiltersL[i].stage2.a2 = (1.0f - sqrt2 * k + k2) * norm;

        lpFiltersR[i].stage1.b0 = lpFiltersR[i].stage2.b0 = k2 * norm;
        lpFiltersR[i].stage1.b1 = lpFiltersR[i].stage2.b1 = 2.0f * k2 * norm;
        lpFiltersR[i].stage1.b2 = lpFiltersR[i].stage2.b2 = k2 * norm;
        lpFiltersR[i].stage1.a1 = lpFiltersR[i].stage2.a1 = 2.0f * (k2 - 1.0f) * norm;
        lpFiltersR[i].stage1.a2 = lpFiltersR[i].stage2.a2 = (1.0f - sqrt2 * k + k2) * norm;
    }

    // HF limiter LP at 6kHz
    {
        const float fc = static_cast<float>(kHFLimiterFreq);
        const float omega = 2.0f * static_cast<float>(kPi) * fc / sampleRate;
        const float k = std::tan(omega / 2.0f);
        const float k2 = k * k;
        const float norm = 1.0f / (1.0f + sqrt2 * k + k2);

        hfLpFilterL.stage1.b0 = hfLpFilterL.stage2.b0 = k2 * norm;
        hfLpFilterL.stage1.b1 = hfLpFilterL.stage2.b1 = 2.0f * k2 * norm;
        hfLpFilterL.stage1.b2 = hfLpFilterL.stage2.b2 = k2 * norm;
        hfLpFilterL.stage1.a1 = hfLpFilterL.stage2.a1 = 2.0f * (k2 - 1.0f) * norm;
        hfLpFilterL.stage1.a2 = hfLpFilterL.stage2.a2 = (1.0f - sqrt2 * k + k2) * norm;

        hfLpFilterR.stage1.b0 = hfLpFilterR.stage2.b0 = k2 * norm;
        hfLpFilterR.stage1.b1 = hfLpFilterR.stage2.b1 = 2.0f * k2 * norm;
        hfLpFilterR.stage1.b2 = hfLpFilterR.stage2.b2 = k2 * norm;
        hfLpFilterR.stage1.a1 = hfLpFilterR.stage2.a1 = 2.0f * (k2 - 1.0f) * norm;
        hfLpFilterR.stage1.a2 = hfLpFilterR.stage2.a2 = (1.0f - sqrt2 * k + k2) * norm;
    }
}

void DeEsserDSP::updateEnvelopeCoefficients() {
    attackCoeff = 1.0f - std::exp(-1.0f / (kAttackMs * 0.001f * sampleRate));
    releaseCoeff = 1.0f - std::exp(-1.0f / (kReleaseMs * 0.001f * sampleRate));
}

float DeEsserDSP::processIIR(SecondOrderIIR& f, float x) {
    const float y = f.b0 * x + f.b1 * f.x1 + f.b2 * f.x2 - f.a1 * f.y1 - f.a2 * f.y2;
    f.x2 = f.x1;
    f.x1 = x;
    f.y2 = f.y1;
    f.y1 = y;
    return y;
}

float DeEsserDSP::processLR4LowPass(LR4Filter& f, float x) {
    const float s1 = processIIR(f.stage1, x);
    return processIIR(f.stage2, s1);
}

void DeEsserDSP::processBand(int band, float inputL, float inputR,
                             float amount, float thresholdLinear,
                             float& outL, float& outR) {
    const float peakL = std::abs(inputL);
    const float peakR = std::abs(inputR);

    float envL = envelopeL[band];
    float envR = envelopeR[band];

    if (peakL > envL)
        envL += attackCoeff * (peakL - envL);
    else
        envL += releaseCoeff * (peakL - envL);
    if (peakR > envR)
        envR += attackCoeff * (peakR - envR);
    else
        envR += releaseCoeff * (peakR - envR);

    envelopeL[band] = envL;
    envelopeR[band] = envR;

    const float maxEnv = std::max(envL, envR);
    float gainL = 1.0f;
    float gainR = 1.0f;

    if (maxEnv > 1e-8f && maxEnv > thresholdLinear) {
        if (envL < 1e-8f)
            gainL = 1.0f;
        else {
            const float overL = envL / thresholdLinear;
            const float grL = std::pow(overL, -1.0f / kRatio);
            gainL = juce::jlimit(0.01f, 1.0f, 1.0f + (grL - 1.0f) * amount);
        }
        if (envR < 1e-8f)
            gainR = 1.0f;
        else {
            const float overR = envR / thresholdLinear;
            const float grR = std::pow(overR, -1.0f / kRatio);
            gainR = juce::jlimit(0.01f, 1.0f, 1.0f + (grR - 1.0f) * amount);
        }
    }

    outL = inputL * gainL;
    outR = inputR * gainR;
}

void DeEsserDSP::process(juce::AudioBuffer<float>& buffer,
                         float thresholdDb,
                         float bandAmounts[NUM_BANDS],
                         bool hfLimiterEnabled) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    const float thresholdLinear = juce::Decibels::decibelsToGain(thresholdDb);

    auto* left = buffer.getWritePointer(0);
    auto* right = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < numSamples; ++i) {
        const float inL = left[i];
        const float inR = (right != nullptr) ? right[i] : inL;

        float lp2kL = processLR4LowPass(lpFiltersL[0], inL);
        float lp4kL = processLR4LowPass(lpFiltersL[1], inL);
        float lp7kL = processLR4LowPass(lpFiltersL[2], inL);
        float lp10kL = processLR4LowPass(lpFiltersL[3], inL);

        float band1L = lp4kL - lp2kL;
        float band2L = lp7kL - lp4kL;
        float band3L = lp10kL - lp7kL;
        float band4L = inL - lp10kL;

        float lp2kR = processLR4LowPass(lpFiltersR[0], inR);
        float lp4kR = processLR4LowPass(lpFiltersR[1], inR);
        float lp7kR = processLR4LowPass(lpFiltersR[2], inR);
        float lp10kR = processLR4LowPass(lpFiltersR[3], inR);

        float band1R = lp4kR - lp2kR;
        float band2R = lp7kR - lp4kR;
        float band3R = lp10kR - lp7kR;
        float band4R = inR - lp10kR;

        float out1L, out1R, out2L, out2R, out3L, out3R, out4L, out4R;
        processBand(0, band1L, band1R, bandAmounts[0], thresholdLinear, out1L, out1R);
        processBand(1, band2L, band2R, bandAmounts[1], thresholdLinear, out2L, out2R);
        processBand(2, band3L, band3R, bandAmounts[2], thresholdLinear, out3L, out3R);
        processBand(3, band4L, band4R, bandAmounts[3], thresholdLinear, out4L, out4R);

        float sumL = out1L + out2L + out3L + out4L;
        float sumR = out1R + out2R + out3R + out4R;

        if (hfLimiterEnabled) {
            float lpL = processLR4LowPass(hfLpFilterL, sumL);
            float lpR = processLR4LowPass(hfLpFilterR, sumR);
            float hfL = sumL - lpL;
            float hfR = sumR - lpR;
            hfL = std::tanh(hfL);
            hfR = std::tanh(hfR);
            sumL = lpL + hfL;
            sumR = lpR + hfR;
        }

        left[i] = sumL;
        if (right != nullptr)
            right[i] = sumR;
    }
}
