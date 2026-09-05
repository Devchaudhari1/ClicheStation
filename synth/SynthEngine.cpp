
#include "SynthEngine.h"
#include <QDebug>

void SynthEngine::noteOn(int midiNote, int velocity)
{
    Voice* existingVoice = findVoiceForNote(midiNote);

    if (existingVoice != nullptr)
    {
        existingVoice->startNote(midiNote, velocity);
        return;
    }

    Voice* voice = findFreeVoice();

    if (voice == nullptr)
    {
        qDebug() << "No free voice!";
        return;
    }

    voice->startNote(midiNote, velocity);
}

void SynthEngine::render(float* left,
                         float* right,
                         std::size_t numSamples)
{
    std::fill(left, left + numSamples, 0.0f);
    std::fill(right, right + numSamples, 0.0f);

    std::vector<float> voiceBuffer(numSamples);

    for (auto& voice : m_voices)
    {
        if (!voice.isActive())
            continue;

        std::fill(voiceBuffer.begin(), voiceBuffer.end(), 0.0f);

        voice.render(voiceBuffer.data(), numSamples);

        for (std::size_t i = 0; i < numSamples; ++i)
        {
            left[i] += voiceBuffer[i];
            right[i] += voiceBuffer[i];
        }
    }
}


void SynthEngine::noteOff(
    int midiNote)
{
    Voice* voice =
        findVoiceForNote(
            midiNote);

    if (voice == nullptr)
        return;

    voice->stopNote();
}

void SynthEngine::reset()
{
    for (auto& voice : m_voices)
    {
        voice.stopNote();
    }
}

void SynthEngine::prepare(
    double sampleRate,
    std::size_t blockSize)
{
    m_sampleRate = sampleRate;
    m_blockSize = blockSize;

    for (auto& voice : m_voices)
    {
        voice.prepare(
            sampleRate);
    }
}

Voice* SynthEngine::findFreeVoice()
{
    for (auto& voice : m_voices)
    {
        if (!voice.isActive())
            return &voice;
    }

    return nullptr;
}

Voice* SynthEngine::findVoiceForNote(
    int midiNote)
{
    for (auto& voice : m_voices)
    {
        if (voice.isActive() &&
            voice.getMidiNote() == midiNote)
        {
            return &voice;
        }
    }

    return nullptr;
}

void SynthEngine::pitchBend(
    int channel,
    int value)
{
    // MIDI pitch bend is 14-bit:
    // 0     = maximum down
    // 8192  = center
    // 16383 = maximum up

    constexpr float center = 8192.0f;

    m_pitchBend =
        (static_cast<float>(value) - center)
        / center;

    // Clamp to [-1, +1]
    if (m_pitchBend < -1.0f)
        m_pitchBend = -1.0f;

    if (m_pitchBend > 1.0f)
        m_pitchBend = 1.0f;

    // Apply to currently active voices
    for (auto& voice : m_voices)
    {
        if (voice.isActive())
        {
            voice.setPitchBend(
                m_pitchBend);
        }
    }
}