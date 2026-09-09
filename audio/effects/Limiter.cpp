#include "Limiter.h"

#include <algorithm>
#include <cmath>

void Limiter::prepare(double sampleRate,
                      std::size_t,
                      std::size_t)
{
    m_sampleRate = sampleRate;
    reset();
}

void Limiter::process(float *left,
                      float *right,
                      std::size_t numSamples)
{
    const float thresholdDb =
        std::clamp(m_threshold.load(), -60.0f, 0.0f);

    const float release =
        std::clamp(m_release.load(), 0.001f, 1.0f);

    const float ceilingDb =
        std::clamp(m_ceiling.load(), -20.0f, 0.0f);

    const float threshold =
        std::pow(10.0f, thresholdDb / 20.0f);

    const float ceiling =
        std::pow(10.0f, ceilingDb / 20.0f);

    const float releaseCoefficient =
        std::exp(
            -1.0f /
            (release * static_cast<float>(m_sampleRate))
        );

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const float inputPeak =
            std::max(std::abs(left[i]),
                     std::abs(right[i]));

        float targetGain = 1.0f;

        if (inputPeak > threshold)
        {
            targetGain = threshold / inputPeak;
        }

        if (targetGain < m_gain)
        {
            m_gain = targetGain;
        }
        else
        {
            m_gain =
                targetGain +
                (m_gain - targetGain) *
                releaseCoefficient;
        }

        left[i] *= m_gain;
        right[i] *= m_gain;

        left[i] =
            std::clamp(left[i], -ceiling, ceiling);

        right[i] =
            std::clamp(right[i], -ceiling, ceiling);
    }
}

void Limiter::reset()
{
    m_gain = 1.0f;
}

AudioEffectType Limiter::effectType() const
{
    return AudioEffectType::Limiter;
}

QString Limiter::effectName() const
{
    return "Limiter";
}

void Limiter::setThreshold(float threshold)
{
    m_threshold.store(threshold);
}

void Limiter::setRelease(float release)
{
    m_release.store(release);
}

void Limiter::setCeiling(float ceiling)
{
    m_ceiling.store(ceiling);
}