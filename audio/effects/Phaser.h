#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>

class Phaser : public AudioEffect
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
    static constexpr std::size_t m_numStages = 4;

    double m_sampleRate = 48000.0;

    std::atomic<float> m_rate{0.5f};
    std::atomic<float> m_depth{0.7f};
    std::atomic<float> m_feedback{0.2f};
    std::atomic<float> m_mix{0.5f};

    float m_lfoPhase = 0.0f;
    float m_feedbackLeft = 0.0f;
    float m_feedbackRight = 0.0f;

    float m_stateLeft[m_numStages]{};
    float m_stateRight[m_numStages]{};
};