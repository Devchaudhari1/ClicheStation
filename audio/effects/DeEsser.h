#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>

class DeEsser : public AudioEffect
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

    void setFrequency(float frequency);
    void setThreshold(float threshold);
    void setRatio(float ratio);
    void setAttack(float attack);
    void setRelease(float release);
    void setRange(float range);

private:
    double m_sampleRate = 48000.0;

    std::atomic<float> m_frequency{6000.0f};
    std::atomic<float> m_threshold{-24.0f};
    std::atomic<float> m_ratio{4.0f};
    std::atomic<float> m_attack{0.001f};
    std::atomic<float> m_release{0.08f};
    std::atomic<float> m_range{-12.0f};

    // Detector filter state
    float m_highPassStateLeft = 0.0f;
    float m_highPassStateRight = 0.0f;

    float m_lowPassStateLeft = 0.0f;
    float m_lowPassStateRight = 0.0f;

    // Envelope follower state
    float m_envelope = 0.0f;
};