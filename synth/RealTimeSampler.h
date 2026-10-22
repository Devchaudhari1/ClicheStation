#pragma once

#include <cstddef>
#include <vector>

class RealTimeSampler
{
public:
    void prepare(double sampleRate,
                 std::size_t maximumBlockSize,
                 std::size_t channels,
                 double maximumRecordingSeconds);

    void startRecording();
    void stopRecording();
    void reset();

    bool isRecording() const;

    void process(const float *left,
                 const float *right,
                 std::size_t numSamples);

    void trim(std::size_t startSample,
        std::size_t endSample);
        
    const std::vector<float>& leftBuffer() const;
    const std::vector<float>& rightBuffer() const;

    std::size_t numSamples() const;
    double sampleRate() const;

private:
    double m_sampleRate = 48000.0;

    std::size_t m_maximumBlockSize = 0;
    std::size_t m_channels = 2;

    std::size_t m_maximumSamples = 0;
    std::size_t m_numSamples = 0;

    bool m_recording = false;

    std::vector<float> m_leftBuffer;
    std::vector<float> m_rightBuffer;
};