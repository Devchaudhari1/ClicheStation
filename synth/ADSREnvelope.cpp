#include "ADSREnvelope.h"
#include <algorithm>

void ADSREnvelope::setAttack(float seconds)
{
    m_attack = seconds;
}

void ADSREnvelope::setDecay(float seconds)
{
    m_decay = seconds;
}

void ADSREnvelope::setSustain(float level)
{
    m_sustain = std::clamp(level, 0.0f, 1.0f);
}

void ADSREnvelope::setRelease(float seconds)
{
    m_release = seconds;
}

void ADSREnvelope::setPeakGain(float level)
{
    m_peakGain = std::clamp(level, 0.0f, 1.0f);
}

bool ADSREnvelope::isActive() const
{
    return m_stage != Stage::Idle;
}

void ADSREnvelope::noteOn()
{
    m_stage = Stage::Attack;
}
void ADSREnvelope::noteOff()
{
    m_stage = Stage::Release;
}

void ADSREnvelope::reset()
{
    m_stage = Stage::Idle;
    m_level = 0.0f;
}

void ADSREnvelope::prepare(double sampleRate)
{
    m_sampleRate = sampleRate;
}

float ADSREnvelope::processSample()
{
    switch (m_stage)
    {
        case Stage::Attack:
        {
            if (m_attack <= 0.0f)
            {
                m_level = 1.0f;
                m_stage = Stage::Decay;
                break;
            }

            const float increment =
                1.0f / static_cast<float>(m_attack * m_sampleRate);

            m_level += increment;

            if (m_level >= 1.0f)
            {
                m_level = 1.0f;
                m_stage = Stage::Decay;
            }

            break;
        }

        case Stage::Decay:
        {
            if (m_decay <= 0.0f)
            {
                m_level = m_sustain;
                m_stage = Stage::Sustain;
                break;
            }

            const float decrement =
                (1.0f - m_sustain) /
                static_cast<float>(m_decay * m_sampleRate);

            m_level -= decrement;

            if (m_level <= m_sustain)
            {
                m_level = m_sustain;
                m_stage = Stage::Sustain;
            }

            break;
        }

        case Stage::Sustain:
        {
            m_level = m_sustain;
            break;
        }

            case Stage::Release:
        {
            if (m_release <= 0.0f)
            {
                m_level = 0.0f;
                m_stage = Stage::Idle;
                break;
            }

            const float decrement =
                1.0f /
                static_cast<float>(m_release * m_sampleRate);

            m_level -= decrement;

            if (m_level <= 0.0f)
            {
                m_level = 0.0f;
                m_stage = Stage::Idle;
            }

            break;
        }

        case Stage::Idle:
        {
            m_level = 0.0f;
            break;
        }
    }

    return m_level;
}