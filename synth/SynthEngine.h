#pragma once

#include "Voice.h"

#include <array>
#include <cstddef>

class SynthEngine
{
public:

    static constexpr std::size_t
        MaxVoices = 32;
    float m_pitchBend=0.0f;
    void prepare(
        double sampleRate,
        std::size_t blockSize);

    void reset();

    void noteOn(
        int midiNote,
        int velocity);

    void noteOff(
        int midiNote);

    void render(
        float* left,
        float* right,
        std::size_t numSamples);
    
    void pitchBend(
        int channel,
        int value);

private:

    Voice* findFreeVoice();

    Voice* findVoiceForNote(
        int midiNote);

private:

    std::array<
        Voice,
        MaxVoices
    > m_voices;

    double m_sampleRate =
        44100.0;

    std::size_t m_blockSize =
        512;
};