#include "Clipping.h"

#include <algorithm>

void Clipping::prepare(double,
                       std::size_t,
                       std::size_t)
{
}

void Clipping::process(float *left,
                       float *right,
                       std::size_t numSamples)
{
    for (std::size_t i = 0; i < numSamples; ++i)
    {
        left[i] = std::clamp(left[i], -1.0f, 1.0f);
        right[i] = std::clamp(right[i], -1.0f, 1.0f);
    }
}

void Clipping::reset()
{
}

AudioEffectType Clipping::effectType() const
{
    return AudioEffectType::Clipping;
}


QString Clipping::effectName() const
{
    return "Clipping";
}