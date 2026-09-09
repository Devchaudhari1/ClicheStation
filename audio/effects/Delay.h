#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <vector>
#include <cstddef>

class Delay : public AudioEffect
{
public:
    void prepare(double sampleRate,
                 std::size_t maximumBlockSize,
                 std::size_t channels) override;

    void process(float *left,
                 float *right,
                 std::size_t numSamples) override;

    void reset() override;

    QString effectName() const override;
    AudioEffectType effectType() const override;
    void setDelayTime(float seconds);
    void setFeedback(float feedback);
    void setMix(float mix);

private:
    double m_sampleRate = 48000.0;

    std::vector<float> m_leftBuffer;
    std::vector<float> m_rightBuffer;

    std::size_t m_writePosition = 0;

    std::atomic<float> m_delayTime{0.25f};
    std::atomic<float> m_feedback{0.35f};
    std::atomic<float> m_mix{0.35f};
};