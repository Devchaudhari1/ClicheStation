#include "Flanger.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float twoPi = 6.28318530717958647692f;
}

void Flanger::prepare(double sampleRate,
                      std::size_t,
                      std::size_t)
{
    m_sampleRate = sampleRate;

    // We need enough space for the maximum delay.
    const std::size_t maxDelaySamples =
        static_cast<std::size_t>(
            std::ceil(m_maxDelay * m_sampleRate)) + 2;

    m_leftBuffer.assign(maxDelaySamples, 0.0f);
    m_rightBuffer.assign(maxDelaySamples, 0.0f);

    reset();
}

void Flanger::process(float *left,
                      float *right,
                      std::size_t numSamples)
{
    if (!left || !right || numSamples == 0)
        return;

    if (m_leftBuffer.empty() || m_rightBuffer.empty())
        return;

    const float rate = std::clamp(
        m_rate.load(),
        0.01f,
        10.0f);

    const float depth = std::clamp(
        m_depth.load(),
        0.0f,
        m_maxDelay - m_minDelay);

    const float feedback = std::clamp(
        m_feedback.load(),
        -0.95f,
        0.95f);

    const float mix = std::clamp(
        m_mix.load(),
        0.0f,
        1.0f);

    const float bufferSize =
        static_cast<float>(m_leftBuffer.size());

    const float phaseIncrement =
        rate / static_cast<float>(m_sampleRate);

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        // LFO goes from -1 to +1.
        const float lfo =
            std::sin(twoPi * m_lfoPhase);

        // Convert LFO to delay time.
        //
        // 0 -> minimum delay
        // 1 -> maximum delay
        const float delaySeconds =
            m_minDelay +
            ((lfo + 1.0f) * 0.5f) * depth;

        const float delaySamples =
            delaySeconds *
            static_cast<float>(m_sampleRate);

        // Write the current input first.
        const float inputLeft = left[i];
        const float inputRight = right[i];

        const float feedbackLeft =
            inputLeft +
            feedback * (
                m_leftBuffer[m_writePosition]);

        const float feedbackRight =
            inputRight +
            feedback * (
                m_rightBuffer[m_writePosition]);

        m_leftBuffer[m_writePosition] =
            feedbackLeft;

        m_rightBuffer[m_writePosition] =
            feedbackRight;

        // Fractional read position.
        float readPosition =
            static_cast<float>(m_writePosition)
            - delaySamples;

        if (readPosition < 0.0f)
            readPosition += bufferSize;

        const std::size_t index0 =
            static_cast<std::size_t>(readPosition);

        const std::size_t index1 =
            (index0 + 1) % m_leftBuffer.size();

        const float fraction =
            readPosition -
            static_cast<float>(index0);

        // Linear interpolation gives us a fractional delay.
        const float delayedLeft =
            m_leftBuffer[index0] +
            fraction *
            (m_leftBuffer[index1] -
             m_leftBuffer[index0]);

        const float delayedRight =
            m_rightBuffer[index0] +
            fraction *
            (m_rightBuffer[index1] -
             m_rightBuffer[index0]);
        
        m_leftBuffer[m_writePosition] =
            inputLeft + delayedLeft * feedback;

        m_rightBuffer[m_writePosition] =
            inputRight + delayedRight * feedback;
            
        // Wet/dry mix.
        left[i] =
            inputLeft * (1.0f - mix) +
            delayedLeft * mix;

        right[i] =
            inputRight * (1.0f - mix) +
            delayedRight * mix;

        // Advance circular buffer.
        m_writePosition++;

        if (m_writePosition >= m_leftBuffer.size())
            m_writePosition = 0;

        // Advance LFO.
        m_lfoPhase += phaseIncrement;

        if (m_lfoPhase >= 1.0f)
            m_lfoPhase -= 1.0f;
    }
}

void Flanger::reset()
{
    std::fill(
        m_leftBuffer.begin(),
        m_leftBuffer.end(),
        0.0f);

    std::fill(
        m_rightBuffer.begin(),
        m_rightBuffer.end(),
        0.0f);

    m_writePosition = 0;
    m_lfoPhase = 0.0f;
}

AudioEffectType Flanger::effectType() const
{
    return AudioEffectType::Flanger;
}

QString Flanger::effectName() const
{
    return "Flanger";
}

void Flanger::setRate(float rate)
{
    m_rate.store(rate);
}

void Flanger::setDepth(float depth)
{
    m_depth.store(depth);
}

void Flanger::setFeedback(float feedback)
{
    m_feedback.store(feedback);
}

void Flanger::setMix(float mix)
{
    m_mix.store(mix);
}
