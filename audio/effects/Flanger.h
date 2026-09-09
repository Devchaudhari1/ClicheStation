#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>
#include <vector>

class Flanger : public AudioEffect
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
    void setFeedback(float feedback);
    void setMix(float mix);

private:
    double m_sampleRate = 48000.0;

    std::vector<float> m_leftBuffer;
    std::vector<float> m_rightBuffer;

    std::size_t m_writePosition = 0;

    // LFO state.
    float m_lfoPhase = 0.0f;

    // Parameters.
    std::atomic<float> m_rate{0.25f};
    std::atomic<float> m_depth{0.002f};
    std::atomic<float> m_feedback{0.2f};
    std::atomic<float> m_mix{0.5f};

    static constexpr float m_minDelay = 0.001f; // 1 ms
    static constexpr float m_maxDelay = 0.005f; // 5 ms
};