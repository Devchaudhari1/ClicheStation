#include "Compression.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float minThreshold = -60.0f;
constexpr float maxThreshold = 0.0f;

constexpr float minRatio = 1.0f;
constexpr float maxRatio = 20.0f;

constexpr float minTime = 0.0001f;
constexpr float maxTime = 1.0f;

constexpr float minMakeupGain = 0.0f;
constexpr float maxMakeupGain = 10.0f;

constexpr float epsilon = 1.0e-8f;
}

void Compression::prepare(double sampleRate,
                          std::size_t,
                          std::size_t)
{
    m_sampleRate = sampleRate;

    updateEnvelopeCoefficients();
    reset();
}

void Compression::updateEnvelopeCoefficients()
{
    const float attack =
        std::clamp(
            m_attack.load(),
            minTime,
            maxTime);

    const float release =
        std::clamp(
            m_release.load(),
            minTime,
            maxTime);

    m_attackCoefficient =
        std::exp(
            -1.0f /
            (attack *
             static_cast<float>(m_sampleRate)));

    m_releaseCoefficient =
        std::exp(
            -1.0f /
            (release *
             static_cast<float>(m_sampleRate)));
}

void Compression::process(float *left,
                          float *right,
                          std::size_t numSamples)
{
    if (!left || !right || numSamples == 0)
        return;

    const float threshold =
        std::clamp(
            m_threshold.load(),
            minThreshold,
            maxThreshold);

    const float ratio =
        std::clamp(
            m_ratio.load(),
            minRatio,
            maxRatio);

    const float makeupGain =
        std::clamp(
            m_makeupGain.load(),
            minMakeupGain,
            maxMakeupGain);
    const float attack =
        std::clamp(m_attack.load(),
            minTime,
            maxTime);

    const float release =
        std::clamp(m_release.load(), 
        minTime, 
        maxTime);

    const float attackCoefficient =
        std::exp(
            -1.0f /
            (attack * static_cast<float>(m_sampleRate)));

    const float releaseCoefficient =
        std::exp(
            -1.0f /
            (release * static_cast<float>(m_sampleRate)));
    // Parameters can be changed from the GUI while
    // the audio thread is running.
    updateEnvelopeCoefficients();

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const float inputLeft = left[i];
        const float inputRight = right[i];

        // Stereo-linked detector.
        const float detector =
            std::max(
                std::abs(inputLeft),
                std::abs(inputRight));

        // Envelope follower.
        if (detector > m_envelope)
        {
            m_envelope =
                m_attackCoefficient * m_envelope +
                (1.0f - m_attackCoefficient) * detector;
        }
        else
        {
            m_envelope =
                m_releaseCoefficient * m_envelope +
                (1.0f - m_releaseCoefficient) * detector;
        }

        // Avoid log10(0).
        const float envelope =
            std::max(m_envelope, epsilon);

        // Convert envelope to dB.
        const float inputDb =
            20.0f * std::log10(envelope);

        float gainReductionDb = 0.0f;

        if (inputDb > threshold)
        {
            // Amount above threshold.
            const float aboveThreshold =
                inputDb - threshold;

            // Compression curve.
            const float compressedLevel =
                aboveThreshold / ratio;

            gainReductionDb =
                compressedLevel -
                aboveThreshold;
        }

        // Convert gain reduction from dB to linear.
        const float compressionGain =
            std::pow(
                10.0f,
                gainReductionDb / 20.0f);

        // Makeup gain.
        const float outputGain =
            compressionGain * makeupGain;

        left[i] =
            inputLeft * outputGain;

        right[i] =
            inputRight * outputGain;
    }
}

void Compression::reset()
{
    m_envelope = 0.0f;
}

AudioEffectType Compression::effectType() const
{
    return AudioEffectType::Compression;
}

QString Compression::effectName() const
{
    return "Compression";
}

void Compression::setThreshold(float threshold)
{
    m_threshold.store(threshold);
}

void Compression::setRatio(float ratio)
{
    m_ratio.store(ratio);
}

void Compression::setAttack(float attack)
{
    m_attack.store(attack);
    updateEnvelopeCoefficients();
}

void Compression::setRelease(float release)
{
    m_release.store(release);
    updateEnvelopeCoefficients();
}

void Compression::setMakeupGain(float gain)
{
    m_makeupGain.store(gain);
}