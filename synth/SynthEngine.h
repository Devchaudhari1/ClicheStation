#pragma once

#include "Voice.h"
#include "SampleVoice.h"
#include "SampleBank.h"
#include "DrumSampleBank.h"
#include <array>
#include <cstddef>
#include <QVector>
#include "../gui/pianoRoll.h"
#include "../gui/DrumSequence.h"
#include "SynthesizersSamples.h"
#include "DrumsetsSamples.h"
class SynthEngine
{
public:

    static constexpr std::size_t MaxVoices = 32;

    float m_pitchBend = 0.0f;

    void prepare(double sampleRate, std::size_t blockSize);
    void reset();

    // Normal oscillator / sample instrument
    void noteOn(int midiNote, int velocity);
    void noteOff(int midiNote);

    void sampleNoteOn(int midiNote, int velocity);
    void sampleNoteOff(int midiNote);

    // Drums
    void drumNoteOn(DrumsetsSamples drum, int midiNote, int velocity);

    // Rendering
    void render(
        float* left,
        float* right,
        std::size_t numSamples);

    void render(
        const QVector<PlacedNote>& notes,
        QVector<float>& left,
        QVector<float>& right);

    void render(
        const QVector<DrumStepEvent>& drumSteps,
        double secondsPerStep,
        QVector<float>& left,
        QVector<float>& right);
        
    void pitchBend(int channel, int value);

    // Sample instrument
    void setSampleInstrument(
        SynthesizersSamples sample);

    // Drum instrument
    void setDrumInstrument(
        DrumsetsSamples drum, int rootMidiNote);

    // Oscillator
    void setFrequency(float frequency);
    void setVolume(float volume);
    void setWaveform(Oscillator::Waveform waveform);
    void setVelocity(float velocity);

    // ADSR
    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);
    void setRelease(float seconds);
    void setPeakGain(float level);

private:

    Voice* findFreeVoice();
    Voice* findVoiceForNote(int midiNote);

    SampleVoice* findFreeSampleVoice();
    SampleVoice* findSampleVoiceForNote(int midiNote);
    SynthesizersSamples m_sampleInstrument =
        SynthesizersSamples::None;

    DrumsetsSamples m_drumInstrument =
        DrumsetsSamples::None;

    std::array<Voice, MaxVoices> m_voices;
    std::array<SampleVoice, MaxVoices> m_sampleVoices;

    SampleBank m_sampleBank;
    DrumSampleBank m_drumSampleBank;

    double m_sampleRate = 44100.0;
    std::size_t m_blockSize = 512;
};