#include "Clipping.h"
#include "AudioEffect.h"
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
    const float threshold =
        std::clamp(m_threshold.load(), 0.01f, 1.0f);

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        left[i] = std::clamp(left[i], -threshold, threshold);
        right[i] = std::clamp(right[i], -threshold, threshold);
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

void Clipping::setThreshold(float threshold)
{
    m_threshold.store(threshold);
}