#include "Chorus.h"

#include <algorithm>
#include <cmath>

void Chorus::prepare(double sampleRate,
                     std::size_t,
                     std::size_t)
{
    m_sampleRate = sampleRate;

    const std::size_t bufferSize =
        static_cast<std::size_t>(
            m_maxDelay * m_sampleRate) + 2;

    m_leftBuffer.assign(bufferSize, 0.0f);
    m_rightBuffer.assign(bufferSize, 0.0f);

    reset();
}

void Chorus::process(float *left,
                     float *right,
                     std::size_t numSamples)
{
    if (m_leftBuffer.empty() || m_rightBuffer.empty())
        return;

    const float rate =
        std::clamp(m_rate.load(), 0.01f, 10.0f);

    const float depth =
        std::clamp(m_depth.load(), 0.0f, 0.020f);

    const float mix =
        std::clamp(m_mix.load(), 0.0f, 1.0f);

    const float bufferSize =
        static_cast<float>(m_leftBuffer.size());

    const float baseDelay =
        0.020f;

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const float lfo =
            0.5f *
            (1.0f + std::sin(2.0f *
                             static_cast<float>(M_PI) *
                             m_lfoPhase));

        const float delaySeconds =
            baseDelay + depth * (lfo - 0.5f);

        const float delaySamples =
            delaySeconds *
            static_cast<float>(m_sampleRate);

        const float readPosition =
            static_cast<float>(m_writePosition) -
            delaySamples;

        float wrappedPosition = readPosition;

        while (wrappedPosition < 0.0f)
            wrappedPosition += bufferSize;

        const std::size_t index0 =
            static_cast<std::size_t>(wrappedPosition);

        const std::size_t index1 =
            (index0 + 1) %
            m_leftBuffer.size();

        const float fraction =
            wrappedPosition -
            static_cast<float>(index0);

        const float delayedLeft =
            m_leftBuffer[index0] * (1.0f - fraction) +
            m_leftBuffer[index1] * fraction;

        const float delayedRight =
            m_rightBuffer[index0] * (1.0f - fraction) +
            m_rightBuffer[index1] * fraction;

        m_leftBuffer[m_writePosition] = left[i];
        m_rightBuffer[m_writePosition] = right[i];

        left[i] =
            left[i] * (1.0f - mix) +
            delayedLeft * mix;

        right[i] =
            right[i] * (1.0f - mix) +
            delayedRight * mix;

        m_writePosition =
            (m_writePosition + 1) %
            m_leftBuffer.size();

        m_lfoPhase +=
            rate / static_cast<float>(m_sampleRate);

        if (m_lfoPhase >= 1.0f)
            m_lfoPhase -= 1.0f;
    }
}

void Chorus::reset()
{
    std::fill(m_leftBuffer.begin(),
              m_leftBuffer.end(),
              0.0f);

    std::fill(m_rightBuffer.begin(),
              m_rightBuffer.end(),
              0.0f);

    m_writePosition = 0;
    m_lfoPhase = 0.0f;
}

AudioEffectType Chorus::effectType() const
{
    return AudioEffectType::Chorus;
}

QString Chorus::effectName() const
{
    return "Chorus";
}

void Chorus::setRate(float rate)
{
    m_rate.store(rate);
}

void Chorus::setDepth(float depth)
{
    m_depth.store(depth);
}

void Chorus::setMix(float mix)
{
    m_mix.store(mix);
}