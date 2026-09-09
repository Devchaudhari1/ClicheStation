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

    void setClippingThreshold(float threshold);
    
    void setDistortionDrive(float drive);

    void setDelayTime(double time);
    void setDelayFeedback(double feedback);
    void setDelayMix(double mix);

    void setReverbRoomSize(float roomSize);
    void setReverbDamping(float damping);
    void setReverbWet(float wet);
    void setReverbDecay(float decay);
    
    void setSaturationDrive(float drive);

    void setFlangerRate(float rate);
    void setFlangerDepth(float depth);
    void setFlangerFeedback(float feedback);
    void setFlangerMix(float mix);

    void setCompressionThreshold(float threshold);
    void setCompressionRatio(float ratio);
    void setCompressionAttack(float attack);
    void setCompressionRelease(float release);
    void setCompressionMakeupGain(float gain);

    void setEQLowFrequency(float frequency);
    void setEQLowGain(float gain);

    void setEQMidFrequency(float frequency);
    void setEQMidGain(float gain);
    void setEQMidQ(float q);

    void setEQHighFrequency(float frequency);
    void setEQHighGain(float gain);
    
    // Limiter
    void setLimiterThreshold(float threshold);
    void setLimiterRelease(float release);
    void setLimiterCeiling(float ceiling);

    // Gate
    void setGateThreshold(float threshold);
    void setGateAttack(float attack);
    void setGateRelease(float release);
    void setGateRange(float range);

    // Chorus
    void setChorusRate(float rate);
    void setChorusDepth(float depth);
    void setChorusMix(float mix);

    // Phaser
    void setPhaserRate(float rate);
    void setPhaserDepth(float depth);
    void setPhaserFeedback(float feedback);
    void setPhaserMix(float mix);
    
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