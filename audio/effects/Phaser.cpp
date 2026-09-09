#include "Phaser.h"

#include <algorithm>
#include <cmath>

void Phaser::prepare(double sampleRate,
                     std::size_t,
                     std::size_t)
{
    m_sampleRate = sampleRate;
    reset();
}

void Phaser::process(float *left,
                     float *right,
                     std::size_t numSamples)
{
    const float rate =
        std::clamp(m_rate.load(), 0.01f, 10.0f);

    const float depth =
        std::clamp(m_depth.load(), 0.0f, 1.0f);

    const float feedback =
        std::clamp(m_feedback.load(), -0.95f, 0.95f);

    const float mix =
        std::clamp(m_mix.load(), 0.0f, 1.0f);

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const float lfo =
            0.5f *
            (1.0f +
             std::sin(
                 2.0f *
                 static_cast<float>(M_PI) *
                 m_lfoPhase));

        const float minFrequency = 300.0f;
        const float maxFrequency = 5000.0f;

        const float frequency =
            minFrequency +
            depth * lfo *
            (maxFrequency - minFrequency);

        const float omega =
            2.0f *
            static_cast<float>(M_PI) *
            frequency /
            static_cast<float>(m_sampleRate);

        const float coefficient =
            (1.0f - std::sin(omega)) /
            (1.0f + std::sin(omega));

        float wetLeft =
            left[i] +
            m_feedbackLeft * feedback;

        float wetRight =
            right[i] +
            m_feedbackRight * feedback;

        for (std::size_t stage = 0;
             stage < m_numStages;
             ++stage)
        {
            const float outputLeft =
                -coefficient * wetLeft +
                m_stateLeft[stage];

            m_stateLeft[stage] =
                wetLeft +
                coefficient * outputLeft;

            wetLeft = outputLeft;

            const float outputRight =
                -coefficient * wetRight +
                m_stateRight[stage];

            m_stateRight[stage] =
                wetRight +
                coefficient * outputRight;

            wetRight = outputRight;
        }

        m_feedbackLeft = wetLeft;
        m_feedbackRight = wetRight;

        left[i] =
            left[i] * (1.0f - mix) +
            wetLeft * mix;

        right[i] =
            right[i] * (1.0f - mix) +
            wetRight * mix;

        m_lfoPhase +=
            rate / static_cast<float>(m_sampleRate);

        if (m_lfoPhase >= 1.0f)
            m_lfoPhase -= 1.0f;
    }
}

void Phaser::reset()
{
    m_lfoPhase = 0.0f;
    m_feedbackLeft = 0.0f;
    m_feedbackRight = 0.0f;

    for (std::size_t i = 0; i < m_numStages; ++i)
    {
        m_stateLeft[i] = 0.0f;
        m_stateRight[i] = 0.0f;
    }
}

AudioEffectType Phaser::effectType() const
{
    return AudioEffectType::Phaser;
}

QString Phaser::effectName() const
{
    return "Phaser";
}

void Phaser::setRate(float rate)
{
    m_rate.store(rate);
}

void Phaser::setDepth(float depth)
{
    m_depth.store(depth);
}

void Phaser::setFeedback(float feedback)
{
    m_feedback.store(feedback);
}

void Phaser::setMix(float mix)
{
    m_mix.store(mix);
}