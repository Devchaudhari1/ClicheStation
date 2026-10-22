#include "DeEsser.h"

#include <algorithm>
#include <cmath>

void DeEsser::prepare(double sampleRate,
                      std::size_t,
                      std::size_t)
{
    m_sampleRate = sampleRate;
    reset();
}

void DeEsser::process(float *left,
                      float *right,
                      std::size_t numSamples)
{
    const float frequency =
        std::clamp(
            m_frequency.load(),
            2000.0f,
            12000.0f);

    const float thresholdDb =
        std::clamp(
            m_threshold.load(),
            -60.0f,
            0.0f);

    const float ratio =
        std::clamp(
            m_ratio.load(),
            1.0f,
            20.0f);

    const float attack =
        std::clamp(
            m_attack.load(),
            0.0001f,
            1.0f);

    const float release =
        std::clamp(
            m_release.load(),
            0.001f,
            2.0f);

    const float rangeDb =
        std::clamp(
            m_range.load(),
            -40.0f,
            0.0f);

    const float threshold =
        std::pow(
            10.0f,
            thresholdDb / 20.0f);

    const float attackCoefficient =
        std::exp(
            -1.0f /
            (attack *
             static_cast<float>(m_sampleRate)));

    const float releaseCoefficient =
        std::exp(
            -1.0f /
            (release *
             static_cast<float>(m_sampleRate)));

    /*
     * Simple first-order high-pass filter.
     *
     * This isolates the upper-frequency content
     * where most vocal sibilance occurs.
     */
    const float rc =
        1.0f /
        (2.0f * static_cast<float>(M_PI) * frequency);

    const float dt =
        1.0f /
        static_cast<float>(m_sampleRate);

    const float alpha =
        rc / (rc + dt);

    /*
     * Low-pass filter used to remove the very
     * highest frequencies from the detector.
     *
     * This gives us approximately a 4-12 kHz
     * detection region when combined with the
     * high-pass filter.
     */
    const float highCut = frequency * 2.0f;

    const float rcLow =
        1.0f /
        (2.0f * static_cast<float>(M_PI) * highCut);

    const float alphaLow =
        dt / (rcLow + dt);

    const float maximumReduction =
        std::pow(
            10.0f,
            rangeDb / 20.0f);

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        // High-pass filter
        const float highPassLeft =
            alpha *
            (m_highPassStateLeft +
            left[i] -
            m_lowPassStateLeft);

        const float highPassRight =
            alpha *
            (m_highPassStateRight +
            right[i] -
            m_lowPassStateRight);

        m_highPassStateLeft = highPassLeft;
        m_highPassStateRight = highPassRight;


        // Low-pass filter
        m_lowPassStateLeft +=
            alphaLow *
            (highPassLeft - m_lowPassStateLeft);

        m_lowPassStateRight +=
            alphaLow *
            (highPassRight - m_lowPassStateRight);

        const float detectorLeft =
            m_lowPassStateLeft;

        const float detectorRight =
            m_lowPassStateRight;


        // Stereo detector level
        const float detector =
            std::max(
                std::abs(detectorLeft),
                std::abs(detectorRight));

        /*
         * Envelope follower.
         */
        if (detector > m_envelope)
        {
            m_envelope =
                detector +
                (m_envelope - detector) *
                attackCoefficient;
        }
        else
        {
            m_envelope =
                detector +
                (m_envelope - detector) *
                releaseCoefficient;
        }

        /*
         * No reduction below threshold.
         */
        if (m_envelope <= threshold)
            continue;

        const float inputDb =
            20.0f *
            std::log10(
                std::max(
                    m_envelope,
                    0.000001f));

        /*
         * Compressor-style gain reduction.
         */
        const float compressedDb =
            thresholdDb +
            (inputDb - thresholdDb) / ratio;

        const float gainReductionDb =
            compressedDb - inputDb;

        float gain =
            std::pow(
                10.0f,
                gainReductionDb /
                20.0f);

        /*
         * Respect maximum reduction.
         */
        gain =
            std::max(
                gain,
                maximumReduction);

        left[i] *= gain;
        right[i] *= gain;
    }
}

void DeEsser::reset()
{
    m_highPassStateLeft = 0.0f;
    m_highPassStateRight = 0.0f;

    m_lowPassStateLeft = 0.0f;
    m_lowPassStateRight = 0.0f;

    m_envelope = 0.0f;
}

AudioEffectType DeEsser::effectType() const
{
    return AudioEffectType::DeEsser;
}

QString DeEsser::effectName() const
{
    return "De-Esser";
}

void DeEsser::setFrequency(float frequency)
{
    m_frequency.store(frequency);
}

void DeEsser::setThreshold(float threshold)
{
    m_threshold.store(threshold);
}

void DeEsser::setRatio(float ratio)
{
    m_ratio.store(ratio);
}

void DeEsser::setAttack(float attack)
{
    m_attack.store(attack);
}

void DeEsser::setRelease(float release)
{
    m_release.store(release);
}

void DeEsser::setRange(float range)
{
    m_range.store(range);
}