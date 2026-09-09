#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>

class Gate : public AudioEffect
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
    void setAttack(float attack);
    void setRelease(float release);
    void setRange(float range);

private:
    double m_sampleRate = 48000.0;

    std::atomic<float> m_threshold{-40.0f};
    std::atomic<float> m_attack{0.005f};
    std::atomic<float> m_release{0.1f};
    std::atomic<float> m_range{-60.0f};

    float m_gain = 0.0f;
};