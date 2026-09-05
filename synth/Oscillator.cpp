#include "Oscillator.h"

#include <cmath>

namespace
{
    constexpr double TwoPi =
        6.28318530717958647692;
}

/*
 *  Oscillator class generates audio waveforms based on the specified waveform type and frequency.
 * It supports sine, sawtooth, square, and triangle waveforms.
 */
Oscillator::Oscillator()
{
}

void Oscillator::prepare(double sampleRate)
{
    /*sample rate Eg 48kHz sample rate at 440Hz frequency
    A sample is an audio data point like a pixel is to an image
    48kHz sample rate means 48000 samples per second
    440Hz frequency means 440 cycles per second
    48kHz sample rate at 440Hz frequency means 48000 samples per second and 440 cycles per second
    48000 samples per second / 440 cycles per second = 109.09 samples per cycle
    =>109 data points per cycle of the waveform
    */
    m_sampleRate = sampleRate;
}

void Oscillator::reset()
{
    m_phase = 0.0; // resets phase to 0  
}

void Oscillator::setFrequency(float frequency)
{    
    /*
     * \info frequency is in Hz :
     * Eg f in 2πft    m_frequency = frequency;
    */
   m_frequency = frequency;
}

void Oscillator::setWaveform(Waveform waveform)
{
    /*
     * \info waveform is the type of audio waveform to generate
     */
    m_waveform = waveform;
}

float Oscillator::processSample()
{
    float output = 0.0f;

    switch (m_waveform)
    {
        case Waveform::Sine:
            output =
                std::sin(m_phase);
            break;

        case Waveform::Saw:
            output =
                2.0f *
                static_cast<float>(
                    m_phase / TwoPi) - 1.0f;
            break;

        case Waveform::Square:
            output =
                m_phase < M_PI
                    ? 1.0f
                    : -1.0f;
            break;

        case Waveform::Triangle:
        {
            float phase =
                static_cast<float>(
                    m_phase / TwoPi);

            output =
                4.0f *
                std::abs(
                    phase -
                    std::floor(
                        phase + 0.5f))
                - 1.0f;

            break;
        }
    }

    m_phase +=
        TwoPi *
        m_frequency /
        m_sampleRate;
    /*
     * \info phase increment is per sample/data point basis m_frequency with m_sampleRate can be
     * divided into  m_sampleRate/m_frequency data points each giving a phase shift of 2π m_frequency/m_sampleRate
     */
    if (m_phase >= TwoPi)
        m_phase -= TwoPi;

    return output;
}