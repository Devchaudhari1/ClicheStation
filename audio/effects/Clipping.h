#pragma once

#include "AudioEffect.h"

class Clipping : public AudioEffect
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
};