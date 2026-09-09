#include "Oscillator.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr double TwoPi =
        6.28318530717958647692;

    constexpr double Pi =
        TwoPi * 0.5;
}

/*
 * Oscillator class generates audio waveforms based on the specified
 * waveform type and frequency.
 *
 * It supports sine, sawtooth, square, and triangle waveforms.
 */
Oscillator::Oscillator()
{
}

void Oscillator::prepare(double sampleRate)
{
    /*
     * Sample rate determines how many audio samples are generated
     * per second.
     *
     * Example:
     *
     * 48000 Hz sample rate
     * 440 Hz oscillator frequency
     *
     * 48000 / 440 = approximately 109 samples per cycle.
     */
    setSampleRate(sampleRate);
}

void Oscillator::reset()
{
    m_phase = 0.0;
}

void Oscillator::setSampleRate(double sampleRate)
{
    if (sampleRate > 0.0)
        m_sampleRate = sampleRate;
}

void Oscillator::setFrequency(float frequency)
{
    if (frequency >= 0.0f)
        m_frequency = frequency;
}

void Oscillator::setVolume(float volume)
{
    m_volume = std::clamp(volume,0.0f,1.0f);
}

void Oscillator::setWaveform(Waveform waveform)
{
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
                m_phase < Pi
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

    if (m_phase >= TwoPi)
        m_phase -= TwoPi;

    /*
     * Oscillator volume is applied here.
     *
     * Voice velocity is deliberately NOT applied here.
     * Velocity belongs to Voice because it is note-specific.
     */
    return output * m_volume;
}