#include <cmath>
#include <algorithm>
#include "Voice.h"
#include <QDebug>


Voice::Voice()
{

}

void Voice::prepare(
    double sampleRate)
{
    m_oscillator.prepare(
        sampleRate);

    m_envelope.prepare(
        sampleRate);
}


float midiNoteToFrequency(int note)
{
    return 440.0f *
           std::pow(
               2.0f,
               (note - 69) / 12.0f);
}

bool Voice::isActive() const
{
    return m_active;
}

void Voice::startNote(
    int midiNote,
    int velocity)
{
    m_midiNote = midiNote;

    m_velocity =
        velocity / 127.0f;

    float frequency =
        midiNoteToFrequency(
            midiNote);

    m_oscillator.setFrequency(
        frequency);

    m_envelope.noteOn();

    m_active = true;
}


void Voice::stopNote()
{
    qDebug() << "Voice stop:"
             << m_midiNote;

    m_envelope.noteOff();

    if (!m_envelope.isActive())
    {
        qDebug() << "Voice became inactive:"
                 << m_midiNote;

        m_active = false;
    }
}
void Voice::render(
    float* output,
    std::size_t numSamples)
{
    for (std::size_t i = 0;
         i < numSamples;
         ++i)
    {
        float oscillator =
            m_oscillator.processSample();

        float envelope =
            m_envelope.processSample();

        output[i] +=
            oscillator *
            envelope *
            m_velocity;
    }

    if (!m_envelope.isActive())
        m_active = false;
}

void Voice::updateFrequency()
{
    if (m_midiNote < 0)
        return;

    float semitoneOffset =
        m_pitchBend *
        m_pitchBendRange;

    float frequency =
        440.0f *
        std::pow(
            2.0f,
            (m_midiNote - 69) / 12.0f
            + semitoneOffset / 12.0f);

    m_oscillator.setFrequency(frequency);
}

void Voice::setPitchBend(float bend)
{
    // Keep bend in the range [-1, +1]
    m_pitchBend =
        std::clamp(bend, -1.0f, 1.0f);

    updateFrequency();
}

int Voice::getMidiNote() const
{
    return m_midiNote;
}