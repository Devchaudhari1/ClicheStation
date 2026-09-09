#include "EQ.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float pi = 3.14159265358979323846f;
}

float EQ::Biquad::processLeft(float input)
{
    const float output =
        b0 * input + z1Left;

    z1Left =
        b1 * input
        - a1 * output
        + z2Left;

    z2Left =
        b2 * input
        - a2 * output;

    return output;
}

float EQ::Biquad::processRight(float input)
{
    const float output =
        b0 * input + z1Right;

    z1Right =
        b1 * input
        - a1 * output
        + z2Right;

    z2Right =
        b2 * input
        - a2 * output;

    return output;
}

void EQ::Biquad::reset()
{
    z1Left = 0.0f;
    z2Left = 0.0f;

    z1Right = 0.0f;
    z2Right = 0.0f;
}

void EQ::prepare(double sampleRate,
                 std::size_t,
                 std::size_t)
{
    m_sampleRate = sampleRate;

    updateFilters();
    reset();

    m_parametersChanged.store(false);
}

void EQ::process(float *left,
                 float *right,
                 std::size_t numSamples)
{
    if (m_parametersChanged.exchange(false))
        updateFilters();

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        left[i] =
            m_highFilter.processLeft(
                m_midFilter.processLeft(
                    m_lowFilter.processLeft(left[i])));

        right[i] =
            m_highFilter.processRight(
                m_midFilter.processRight(
                    m_lowFilter.processRight(right[i])));
    }
}
void EQ::reset()
{
    m_lowFilter.reset();
    m_midFilter.reset();
    m_highFilter.reset();
}

AudioEffectType EQ::effectType() const
{
    return AudioEffectType::EQ;
}

QString EQ::effectName() const
{
    return "EQ";
}

void EQ::updateFilters()
{
    const float lowFrequency =
        std::clamp(
            m_lowFrequency.load(),
            20.0f,
            static_cast<float>(m_sampleRate * 0.45));

    const float lowGain =
        std::clamp(m_lowGain.load(), -24.0f, 24.0f);

    const float midFrequency =
        std::clamp(
            m_midFrequency.load(),
            20.0f,
            static_cast<float>(m_sampleRate * 0.45));

    const float midGain =
        std::clamp(m_midGain.load(), -24.0f, 24.0f);

    const float midQ =
        std::clamp(m_midQ.load(), 0.1f, 10.0f);

    const float highFrequency =
        std::clamp(
            m_highFrequency.load(),
            20.0f,
            static_cast<float>(m_sampleRate * 0.45));

    const float highGain =
        std::clamp(m_highGain.load(), -24.0f, 24.0f);

    /*
     * Low shelf
     */
    {
        const float A =
            std::pow(10.0f, lowGain / 40.0f);

        const float omega =
            2.0f * pi * lowFrequency /
            static_cast<float>(m_sampleRate);

        const float alpha =
            std::sin(omega) / (2.0f * 0.7071f);

        const float cosOmega = std::cos(omega);
        const float twoSqrtAAlpha =
            2.0f * std::sqrt(A) * alpha;

        const float sqrtA = std::sqrt(A);

        const float b0 =
            A * ((A + 1.0f)
                 - (A - 1.0f) * cosOmega
                 + twoSqrtAAlpha);

        const float b1 =
            2.0f * A *
            ((A - 1.0f)
             - (A + 1.0f) * cosOmega);

        const float b2 =
            A * ((A + 1.0f)
                 - (A - 1.0f) * cosOmega
                 - twoSqrtAAlpha);

        const float a0 =
            (A + 1.0f)
            + (A - 1.0f) * cosOmega
            + twoSqrtAAlpha;

        const float a1 =
            -2.0f *
            ((A - 1.0f)
             + (A + 1.0f) * cosOmega);

        const float a2 =
            (A + 1.0f)
            + (A - 1.0f) * cosOmega
            - twoSqrtAAlpha;

        m_lowFilter.b0 = b0 / a0;
        m_lowFilter.b1 = b1 / a0;
        m_lowFilter.b2 = b2 / a0;
        m_lowFilter.a1 = a1 / a0;
        m_lowFilter.a2 = a2 / a0;

        (void)sqrtA;
    }

    /*
     * Mid peaking EQ
     */
    {
        const float A =
            std::pow(10.0f, midGain / 40.0f);

        const float omega =
            2.0f * pi * midFrequency /
            static_cast<float>(m_sampleRate);

        const float alpha =
            std::sin(omega) / (2.0f * midQ);

        const float cosOmega = std::cos(omega);

        const float b0 =
            1.0f + alpha * A;

        const float b1 =
            -2.0f * cosOmega;

        const float b2 =
            1.0f - alpha * A;

        const float a0 =
            1.0f + alpha / A;

        const float a1 =
            -2.0f * cosOmega;

        const float a2 =
            1.0f - alpha / A;

        m_midFilter.b0 = b0 / a0;
        m_midFilter.b1 = b1 / a0;
        m_midFilter.b2 = b2 / a0;
        m_midFilter.a1 = a1 / a0;
        m_midFilter.a2 = a2 / a0;
    }

    /*
     * High shelf
     */
    {
        const float A =
            std::pow(10.0f, highGain / 40.0f);

        const float omega =
            2.0f * pi * highFrequency /
            static_cast<float>(m_sampleRate);

        const float alpha =
            std::sin(omega) / (2.0f * 0.7071f);

        const float cosOmega = std::cos(omega);
        const float twoSqrtAAlpha =
            2.0f * std::sqrt(A) * alpha;

        const float b0 =
            A * ((A + 1.0f)
                 + (A - 1.0f) * cosOmega
                 + twoSqrtAAlpha);

        const float b1 =
            -2.0f * A *
            ((A - 1.0f)
             + (A + 1.0f) * cosOmega);

        const float b2 =
            A * ((A + 1.0f)
                 + (A - 1.0f) * cosOmega
                 - twoSqrtAAlpha);

        const float a0 =
            (A + 1.0f)
            - (A - 1.0f) * cosOmega
            + twoSqrtAAlpha;

        const float a1 =
            2.0f *
            ((A - 1.0f)
             - (A + 1.0f) * cosOmega);

        const float a2 =
            (A + 1.0f)
            - (A - 1.0f) * cosOmega
            - twoSqrtAAlpha;

        m_highFilter.b0 = b0 / a0;
        m_highFilter.b1 = b1 / a0;
        m_highFilter.b2 = b2 / a0;
        m_highFilter.a1 = a1 / a0;
        m_highFilter.a2 = a2 / a0;
    }
}

void EQ::setLowFrequency(float frequency)
{
    m_lowFrequency.store(frequency);
    m_parametersChanged.store(true);
}

void EQ::setLowGain(float gain)
{
    m_lowGain.store(gain);
    m_parametersChanged.store(true);
}

void EQ::setMidFrequency(float frequency)
{
    m_midFrequency.store(frequency);
    m_parametersChanged.store(true);
}

void EQ::setMidGain(float gain)
{
    m_midGain.store(gain);
    m_parametersChanged.store(true);
}

void EQ::setMidQ(float q)
{
    m_midQ.store(q);
    m_parametersChanged.store(true);
}

void EQ::setHighFrequency(float frequency)
{
    m_highFrequency.store(frequency);
    m_parametersChanged.store(true);
}

void EQ::setHighGain(float gain)
{
    m_highGain.store(gain);
    m_parametersChanged.store(true);
}