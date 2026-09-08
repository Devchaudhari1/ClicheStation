#pragma once

#include "../synth/SynthEngine.h"
#include "effects/EffectChain.h"
#include <QString>
#include <QVector>

class TrackProcessor
{
public:
    explicit TrackProcessor(int trackId);

    void prepare(double sampleRate, std::size_t blockSize);

    void noteOn(int midiNote, int velocity);
    void noteOff(int midiNote);
    void pitchBend(int channel, int value);

    void render(float *left,
                float *right,
                std::size_t numSamples);

    int trackId() const;

    void setDistortionDrive(float drive);
    
    void setEffectEnabled(
        AudioEffectType effectType,
        bool enabled
    );

    void setEffectOrder(
        const QVector<AudioEffectType> &order
    );
private:
    int m_trackId;

    SynthEngine m_synth;
    EffectChain m_effectChain;
};