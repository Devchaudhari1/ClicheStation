#include "Saturation.h"

#include <cmath>

void Saturation::prepare(double,
                         std::size_t,
                         std::size_t)
{
}

void Saturation::process(float *left,
                         float *right,
                         std::size_t numSamples)
{
    const float drive = m_drive.load();

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        left[i] = std::tanh(left[i] * drive);
        right[i] = std::tanh(right[i] * drive);
    }
}

void Saturation::reset()
{
}

void Saturation::setDrive(float drive)
{
    m_drive.store(drive);
}

AudioEffectType Saturation::effectType() const
{
    return AudioEffectType::Saturation;
}

QString Saturation::effectName() const
{
    return "Saturation";
}