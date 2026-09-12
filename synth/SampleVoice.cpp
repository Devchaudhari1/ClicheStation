#include "SampleVoice.h"

#include <cmath>

void SampleVoice::prepare(double sampleRate)
{
    m_outputSampleRate = sampleRate;
}

void SampleVoice::start(
    const SampleData* sample,
    int midiNote,
    int velocity)
{
    if (sample == nullptr || !sample->isValid())
        return;

    m_sample = sample;
    m_midiNote = midiNote;

    m_position = 0.0;

    const double semitones =
        static_cast<double>(midiNote - sample->rootMidiNote);

    const double pitchRatio =
        std::pow(2.0, semitones / 12.0);

    const double sampleRateRatio =
        static_cast<double>(sample->sampleRate)
        / m_outputSampleRate;

    m_playbackRate =
        pitchRatio * sampleRateRatio;

    m_gain =
        static_cast<float>(velocity) / 127.0f;

    m_active = true;
}

void SampleVoice::stop()
{
    m_active = false;
    m_sample = nullptr;
    m_midiNote = -1;
    m_position = 0.0;
}

bool SampleVoice::isActive() const
{
    return m_active;
}

int SampleVoice::midiNote() const
{
    return m_midiNote;
}

void SampleVoice::render(
    float* left,
    float* right,
    std::size_t numSamples)
{
    if (!m_active || m_sample == nullptr)
        return;

    const std::size_t sampleCount =
        m_sample->sampleCount();

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        if (m_position >= sampleCount)
        {
            m_active = false;
            break;
        }

        const std::size_t index =
            static_cast<std::size_t>(m_position);

        const double fraction =
            m_position - static_cast<double>(index);

        float leftSample =
            m_sample->left[index];

        float rightSample =
            m_sample->right[index];

        if (index + 1 < sampleCount)
        {
            leftSample =
                static_cast<float>(
                    m_sample->left[index] * (1.0 - fraction)
                    + m_sample->left[index + 1] * fraction
                );

            rightSample =
                static_cast<float>(
                    m_sample->right[index] * (1.0 - fraction)
                    + m_sample->right[index + 1] * fraction
                );


                
        }

        left[i] += leftSample * m_gain;
        right[i] += rightSample * m_gain;

        m_position += m_playbackRate;
    }
}