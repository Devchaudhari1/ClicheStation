#include "Delay.h"

#include <algorithm>

void Delay::prepare(double sampleRate,
                    std::size_t,
                    std::size_t)
{
    m_sampleRate = sampleRate;

    constexpr float maxDelaySeconds = 2.0f;

    const std::size_t bufferSize =
        static_cast<std::size_t>(m_sampleRate * maxDelaySeconds) + 1;

    m_leftBuffer.assign(bufferSize, 0.0f);
    m_rightBuffer.assign(bufferSize, 0.0f);

    m_writePosition = 0;
}

void Delay::process(float *left,
                    float *right,
                    std::size_t numSamples)
{
    if (m_leftBuffer.empty() || m_rightBuffer.empty())
        return;

    const float delayTime = m_delayTime.load();
    const float feedback = m_feedback.load();
    const float mix = m_mix.load();

    const std::size_t bufferSize = m_leftBuffer.size();

    std::size_t delaySamples =
        static_cast<std::size_t>(delayTime * m_sampleRate);

    delaySamples =
        std::min(delaySamples, bufferSize - 1);

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const std::size_t readPosition =
            (m_writePosition + bufferSize - delaySamples)
            % bufferSize;

        const float delayedLeft =
            m_leftBuffer[readPosition];

        const float delayedRight =
            m_rightBuffer[readPosition];

        const float inputLeft = left[i];
        const float inputRight = right[i];

        m_leftBuffer[m_writePosition] =
            inputLeft + delayedLeft * feedback;

        m_rightBuffer[m_writePosition] =
            inputRight + delayedRight * feedback;

        left[i] =
            inputLeft * (1.0f - mix)
            + delayedLeft * mix;

        right[i] =
            inputRight * (1.0f - mix)
            + delayedRight * mix;

        m_writePosition++;

        if (m_writePosition >= bufferSize)
            m_writePosition = 0;
    }
}

void Delay::reset()
{
    std::fill(m_leftBuffer.begin(),
              m_leftBuffer.end(),
              0.0f);

    std::fill(m_rightBuffer.begin(),
              m_rightBuffer.end(),
              0.0f);

    m_writePosition = 0;
}

void Delay::setDelayTime(float seconds)
{
    seconds = std::clamp(seconds, 0.001f, 2.0f);
    m_delayTime.store(seconds);
}

void Delay::setFeedback(float feedback)
{
    feedback = std::clamp(feedback, 0.0f, 0.95f);
    m_feedback.store(feedback);
}

void Delay::setMix(float mix)
{
    mix = std::clamp(mix, 0.0f, 1.0f);
    m_mix.store(mix);
}

QString Delay::effectName() const
{
    return "Delay";
}

AudioEffectType Delay::effectType() const
{
    return AudioEffectType::Delay;
}

