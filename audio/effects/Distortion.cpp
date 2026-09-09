#include "Distortion.h"

#include <cmath>
#include <QDebug>

void Distortion::prepare(
    double,
    std::size_t,
    std::size_t)
{
}

void Distortion::reset()
{
}

void Distortion::setDrive(float drive)
{
    m_drive.store(drive);
}

void Distortion::process(
    float* left,
    float* right,
    std::size_t numSamples)
{
    const float drive =
        m_drive.load();

    for (std::size_t i = 0;
         i < numSamples;
         ++i)
    {
        left[i] =
            std::tanh(left[i] * drive);

        right[i] =
            std::tanh(right[i] * drive);
    }

}

AudioEffectType Distortion::effectType() const
{
    return AudioEffectType::Distortion;
}

QString Distortion::effectName() const
{
    return "Distortion";
}