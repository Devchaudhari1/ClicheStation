#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>

class Compression : public AudioEffect
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
    void setRatio(float ratio);
    void setAttack(float attack);
    void setRelease(float release);
    void setMakeupGain(float gain);

private:
    double m_sampleRate = 48000.0;

    std::atomic<float> m_threshold{-18.0f};
    std::atomic<float> m_ratio{4.0f};
    std::atomic<float> m_attack{0.01f};
    std::atomic<float> m_release{0.1f};
    std::atomic<float> m_makeupGain{1.0f};

    float m_envelope = 0.0f;

    float m_attackCoefficient = 0.0f;
    float m_releaseCoefficient = 0.0f;

    void updateEnvelopeCoefficients();
};