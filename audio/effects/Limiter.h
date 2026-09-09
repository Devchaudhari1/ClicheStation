#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>

class Limiter : public AudioEffect
{
public:
    void prepare(double sampleRate,
                 std::size_t maximumBlockSize,
                 std::size_t channels) override;

    void process(float *left,
                 float *right,
                 std::size_t numSamples) override;

    void reset() override;

    AudioEffectType effectType() const override;
    QString effectName() const override;

    void setThreshold(float threshold);
    void setRelease(float release);
    void setCeiling(float ceiling);

private:
    double m_sampleRate = 48000.0;

    std::atomic<float> m_threshold{-6.0f};
    std::atomic<float> m_release{0.05f};
    std::atomic<float> m_ceiling{-0.1f};

    float m_gain = 1.0f;
};