#pragma once

#include "SampleData.h"

#include <cstddef>

class SampleVoice
{
public:
    void prepare(double sampleRate);

    void start(
        const SampleData* sample,
        int midiNote,
        int velocity);

    void stop();

    bool isActive() const;

    int midiNote() const;

    void render(
        float* left,
        float* right,
        std::size_t numSamples);

private:
    const SampleData* m_sample = nullptr;

    int m_midiNote = -1;

    double m_position = 0.0;
    double m_playbackRate = 1.0;

    double m_outputSampleRate = 44100.0;

    float m_gain = 1.0f;

    bool m_active = false;
};