#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>
#include <vector>

class Chorus : public AudioEffect
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

    void setRate(float rate);
    void setDepth(float depth);
    void setMix(float mix);

private:
    double m_sampleRate = 48000.0;

    std::vector<float> m_leftBuffer;
    std::vector<float> m_rightBuffer;

    std::size_t m_writePosition = 0;

    float m_lfoPhase = 0.0f;

    std::atomic<float> m_rate{0.8f};
    std::atomic<float> m_depth{0.015f};
    std::atomic<float> m_mix{0.5f};

    static constexpr float m_minDelay = 0.010f;
    static constexpr float m_maxDelay = 0.030f;
};