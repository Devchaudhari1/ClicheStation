#pragma once

#include "Oscillator.h"
#include "ADSREnvelope.h"

#include <cstddef>

class Voice
{
public:

    Voice();

    void prepare(
        double sampleRate);

    void startNote(
        int midiNote,
        int velocity);

    void stopNote();

    void render(
        float* output,
        std::size_t numSamples);

    void setPitchBend(float bend);
    //Oscillator
    void setFrequency(float frequency);
    void setVolume(float volume);
    void setWaveform(Oscillator::Waveform waveform);
    void setVelocity(float velocity);
    //ADSR
    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);
    void setRelease(float seconds);
    void setPeakGain(float level);

    bool isActive() const;

    int getMidiNote() const;

    void updateFrequency();

private:

    float m_pitchBend = 0.0f;

    float m_pitchBendRange = 2.0f;

    Oscillator m_oscillator;

    ADSREnvelope m_envelope;

    int m_midiNote = -1;

    float m_velocity = 0.0f;

    bool m_active = false;
};