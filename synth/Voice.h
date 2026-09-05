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

    bool isActive() const;

    int getMidiNote() const;

    void updateFrequency();
    
private:
    float m_pitchBend = 0.0f;

    float m_pitchBendRange = 2.0f; // ±2 semitones

    Oscillator m_oscillator;

    ADSREnvelope m_envelope;

    int m_midiNote = -1;

    float m_velocity = 0.0f;

    bool m_active = false;
};