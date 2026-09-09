#include "Gate.h"

#include <algorithm>
#include <cmath>

void Gate::prepare(double sampleRate,
                   std::size_t,
                   std::size_t)
{
    m_sampleRate = sampleRate;
    reset();
}

void Gate::process(float *left,
                   float *right,
                   std::size_t numSamples)
{
    const float thresholdDb =
        std::clamp(m_threshold.load(), -80.0f, 0.0f);

    const float attack =
        std::clamp(m_attack.load(), 0.0001f, 1.0f);

    const float release =
        std::clamp(m_release.load(), 0.001f, 2.0f);

    const float rangeDb =
        std::clamp(m_range.load(), -80.0f, 0.0f);

    const float threshold =
        std::pow(10.0f, thresholdDb / 20.0f);

    const float closedGain =
        std::pow(10.0f, rangeDb / 20.0f);

    const float attackCoefficient =
        std::exp(
            -1.0f /
            (attack * static_cast<float>(m_sampleRate)));

    const float releaseCoefficient =
        std::exp(
            -1.0f /
            (release * static_cast<float>(m_sampleRate)));

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const float inputLevel =
            std::max(std::abs(left[i]),
                     std::abs(right[i]));

        const bool open =
            inputLevel >= threshold;

        const float targetGain =
            open ? 1.0f : closedGain;

        if (targetGain > m_gain)
        {
            m_gain =
                targetGain +
                (m_gain - targetGain) *
                attackCoefficient;
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
    }
}

void Gate::reset()
{
    m_gain = 0.0f;
}

AudioEffectType Gate::effectType() const
{
    return AudioEffectType::Gate;
}

QString Gate::effectName() const
{
    return "Gate";
}

void Gate::setThreshold(float threshold)
{
    m_threshold.store(threshold);
}

void Gate::setAttack(float attack)
{
    m_attack.store(attack);
}

void Gate::setRelease(float release)
{
    m_release.store(release);
}

void Gate::setRange(float range)
{
    m_range.store(range);
}