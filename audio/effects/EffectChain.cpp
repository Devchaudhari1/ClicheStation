// EffectChain.cpp

#include "EffectChain.h"

void EffectChain::addEffect(
    std::unique_ptr<AudioEffect> effect)
{
    effects.push_back(std::move(effect));
}

void EffectChain::prepare(
    double sampleRate,
    std::size_t blockSize,
    std::size_t channels)
{
    for (auto& effect : effects)
    {
        effect->prepare(
            sampleRate,
            blockSize,
            channels);
    }
}

void EffectChain::process(
    float* left,
    float* right,
    std::size_t numSamples)
{
    for (auto& effect : effects)
    {
        if (!effect->isBypassed())
        {
            effect->process(
                left,
                right,
                numSamples);
        }
    }
}

void EffectChain::reset()
{
    for (auto& effect : effects)
        effect->reset();
}