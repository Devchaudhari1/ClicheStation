#pragma once

#include <cstddef>

class Oscillator
{
public:

    enum class Waveform
    {
        Sine,
        Saw,
        Square,
        Triangle
    };

    Oscillator();

    void prepare(double sampleRate);

    void reset();

    void setFrequency(float frequency);

    void setWaveform(Waveform waveform);

    float processSample();

private:

    double m_sampleRate = 44100.0;

    float m_frequency = 440.0f;

    double m_phase = 0.0;

    Waveform m_waveform =
        Waveform::Sine;
};