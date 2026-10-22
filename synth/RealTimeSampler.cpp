#include "RealTimeSampler.h"

void RealTimeSampler::prepare(
    double sampleRate,
    std::size_t maximumBlockSize,
    std::size_t channels,
    double maximumRecordingSeconds)
{
    m_sampleRate = sampleRate;
    m_maximumBlockSize = maximumBlockSize;
    m_channels = channels;

    m_maximumSamples =
        static_cast<std::size_t>(
            m_sampleRate * maximumRecordingSeconds);

    m_leftBuffer.resize(m_maximumSamples);
    m_rightBuffer.resize(m_maximumSamples);

    m_numSamples = 0;
    m_recording = false;
}

void RealTimeSampler::trim(
    std::size_t startSample,
    std::size_t endSample)
{
    if (startSample >= endSample)
        return;

    if (startSample >= m_numSamples)
        return;

    if (endSample > m_numSamples)
        endSample = m_numSamples;

    const std::size_t trimmedSamples =
        endSample - startSample;

    for (std::size_t i = 0; i < trimmedSamples; ++i)
    {
        m_leftBuffer[i] =
            m_leftBuffer[startSample + i];

        m_rightBuffer[i] =
            m_rightBuffer[startSample + i];
    }

    m_numSamples = trimmedSamples;
}

void RealTimeSampler::startRecording()
{
    m_numSamples = 0;
    m_recording = true;
}

void RealTimeSampler::stopRecording()
{
    m_recording = false;
}

void RealTimeSampler::reset()
{
    m_numSamples = 0;
    m_recording = false;
}

bool RealTimeSampler::isRecording() const
{
    return m_recording;
}

void RealTimeSampler::process(
    const float *left,
    const float *right,
    std::size_t numSamples)
{
    if (!m_recording)
        return;

    if (!left || !right || numSamples == 0)
        return;

    const std::size_t remaining =
        m_maximumSamples - m_numSamples;

    const std::size_t samplesToCopy =
        numSamples < remaining
            ? numSamples
            : remaining;

    for (std::size_t i = 0; i < samplesToCopy; ++i)
    {
        m_leftBuffer[m_numSamples + i] = left[i];
        m_rightBuffer[m_numSamples + i] = right[i];
    }

    m_numSamples += samplesToCopy;

    if (m_numSamples >= m_maximumSamples)
        m_recording = false;
}

const std::vector<float>&
RealTimeSampler::leftBuffer() const
{
    return m_leftBuffer;
}

const std::vector<float>&
RealTimeSampler::rightBuffer() const
{
    return m_rightBuffer;
}

std::size_t RealTimeSampler::numSamples() const
{
    return m_numSamples;
}

double RealTimeSampler::sampleRate() const
{
    return m_sampleRate;
}