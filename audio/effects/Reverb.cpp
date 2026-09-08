#include "Reverb.h"

#include <algorithm>

void Reverb::prepare(double sampleRate,
                     std::size_t,
                     std::size_t)
{
    m_sampleRate = sampleRate;

    // Maximum internal delay.
    constexpr float maxDelaySeconds = 2.0f;

    const std::size_t bufferSize =
        static_cast<std::size_t>(
            m_sampleRate * maxDelaySeconds
        ) + 1;

    m_leftBuffer.assign(bufferSize, 0.0f);
    m_rightBuffer.assign(bufferSize, 0.0f);

    m_writePosition = 0;
}

void Reverb::process(float *left,
                     float *right,
                     std::size_t numSamples)
{
    if (m_leftBuffer.empty() || m_rightBuffer.empty())
        return;

    const std::size_t bufferSize = m_leftBuffer.size();

    // Simple preset-derived delay times.
    const std::size_t delay1 =
        static_cast<std::size_t>(
            m_sampleRate * (0.029f + m_roomSize * 0.020f)
        );

    const std::size_t delay2 =
        static_cast<std::size_t>(
            m_sampleRate * (0.043f + m_roomSize * 0.030f)
        );

    const std::size_t delay3 =
        static_cast<std::size_t>(
            m_sampleRate * (0.071f + m_roomSize * 0.040f)
        );

    const std::size_t delay4 =
        static_cast<std::size_t>(
            m_sampleRate * (0.113f + m_roomSize * 0.050f)
        );

    const float feedback =
        std::clamp(m_decay, 0.0f, 0.95f);

    const float wet =
        std::clamp(m_wet, 0.0f, 1.0f);

    const float damping =
        std::clamp(m_damping, 0.0f, 1.0f);

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const float inputLeft = left[i];
        const float inputRight = right[i];

        const std::size_t read1 =
            (m_writePosition + bufferSize - delay1)
            % bufferSize;

        const std::size_t read2 =
            (m_writePosition + bufferSize - delay2)
            % bufferSize;

        const std::size_t read3 =
            (m_writePosition + bufferSize - delay3)
            % bufferSize;

        const std::size_t read4 =
            (m_writePosition + bufferSize - delay4)
            % bufferSize;

        const float delayedLeft =
            (m_leftBuffer[read1]
           + m_leftBuffer[read2]
           + m_leftBuffer[read3]
           + m_leftBuffer[read4]) * 0.25f;

        const float delayedRight =
            (m_rightBuffer[read1]
           + m_rightBuffer[read2]
           + m_rightBuffer[read3]
           + m_rightBuffer[read4]) * 0.25f;

        // Simple damping.
        const float dampedLeft =
            delayedLeft * (1.0f - damping);

        const float dampedRight =
            delayedRight * (1.0f - damping);

        // Feed the reverberated signal back into the buffers.
        m_leftBuffer[m_writePosition] =
            inputLeft + dampedLeft * feedback;

        m_rightBuffer[m_writePosition] =
            inputRight + dampedRight * feedback;

        // Dry/wet mix.
        left[i] =
            inputLeft * (1.0f - wet)
            + delayedLeft * wet;

        right[i] =
            inputRight * (1.0f - wet)
            + delayedRight * wet;

        ++m_writePosition;

        if (m_writePosition >= bufferSize)
            m_writePosition = 0;
    }
}

void Reverb::reset()
{
    std::fill(
        m_leftBuffer.begin(),
        m_leftBuffer.end(),
        0.0f
    );

    std::fill(
        m_rightBuffer.begin(),
        m_rightBuffer.end(),
        0.0f
    );

    m_writePosition = 0;
}

AudioEffectType Reverb::effectType() const
{
    return AudioEffectType::Reverb;
}

QString Reverb::effectName() const
{
    return "Reverb";
}