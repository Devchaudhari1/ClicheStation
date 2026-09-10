#pragma once

#include "Voice.h"

#include <array>
#include <cstddef>
#include <QVector>
#include "../gui/pianoRoll.h"

class SynthEngine
{
public:

    static constexpr std::size_t MaxVoices = 32;
    float m_pitchBend=0.0f;
    void prepare(double sampleRate, std::size_t blockSize);

    void reset();

    void noteOn(int midiNote,int velocity);

    void noteOff(int midiNote);

    void render(
        float* left,
        float* right,
        std::size_t numSamples);

    void render(
        const QVector<PlacedNote>& notes,
        QVector<float>& left,
        QVector<float>& right);

    void pitchBend(int channel,int value);
    //Oscillator
    void setFrequency(float frequency);
    void setVolume(float volume);
    void setWaveform(Oscillator::Waveform waveform);
    void setVelocity(float velocity);
    //ADSR
    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);
    void setRelease(float seconds);
    void setPeakGain(float level);
private:

    Voice* findFreeVoice();

    Voice* findVoiceForNote(
        int midiNote);

private:

    std::array<Voice,MaxVoices> m_voices;

    double m_sampleRate = 44100.0;

    std::size_t m_blockSize = 512;
};