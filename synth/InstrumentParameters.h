#pragma once

#include "Oscillator.h"

struct InstrumentParameters
{
    Oscillator::Waveform waveform =
        Oscillator::Waveform::Sine;

    float frequency = 440.0f;
    float volume = 0.8f;
    float velocity = 1.0f;

    float attack = 0.01f;
    float decay = 0.1f;
    float sustain = 0.8f;
    float release = 0.2f;
};