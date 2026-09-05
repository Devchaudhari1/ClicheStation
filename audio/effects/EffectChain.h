// EffectChain.h

#pragma once

#include "AudioEffect.h"

#include <memory>
#include <vector>

class EffectChain
{
public:
    void addEffect(
        std::unique_ptr<AudioEffect> effect);

    void prepare(
        double sampleRate,
        std::size_t blockSize,
        std::size_t channels);

    void process(
        float* left,
        float* right,
        std::size_t numSamples);

    void reset();

private:
    std::vector<std::unique_ptr<AudioEffect>> effects;
};