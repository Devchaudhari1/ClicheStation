#pragma once

#include "AudioEffect.h"

#include <atomic>
#include <cstddef>

class EQ : public AudioEffect
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

    void setLowFrequency(float frequency);
    void setLowGain(float gain);

    void setMidFrequency(float frequency);
    void setMidGain(float gain);
    void setMidQ(float q);

    void setHighFrequency(float frequency);
    void setHighGain(float gain);

private:
    struct Biquad
    {
        float b0 = 1.0f;
        float b1 = 0.0f;
        float b2 = 0.0f;
        float a1 = 0.0f;
        float a2 = 0.0f;

        float z1Left = 0.0f;
        float z2Left = 0.0f;

        float z1Right = 0.0f;
        float z2Right = 0.0f;

        float processLeft(float input);
        float processRight(float input);

        void reset();
    };

    double m_sampleRate = 48000.0;

    std::atomic<float> m_lowFrequency{100.0f};
    std::atomic<float> m_lowGain{0.0f};

    std::atomic<float> m_midFrequency{1000.0f};
    std::atomic<float> m_midGain{0.0f};
    std::atomic<float> m_midQ{1.0f};

    std::atomic<float> m_highFrequency{8000.0f};
    std::atomic<float> m_highGain{0.0f};

    std::atomic<bool> m_parametersChanged{true};
    
    Biquad m_lowFilter;
    Biquad m_midFilter;
    Biquad m_highFilter;

    void updateFilters();
};