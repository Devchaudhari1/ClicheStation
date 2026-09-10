#pragma once

#include <QObject>
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <vector>
#include <memory>
#include <atomic>
#include <cstdint>
#include <QString>
#include <QVector>
#include <QHash>
#include "TrackProcessor.h"

class AudioEngine;



class AudioOutputDevice : public QIODevice
{
public:
    explicit AudioOutputDevice(AudioEngine *engine);

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;

private:
    AudioEngine *m_engine;
};

class AudioEngine : public QObject
{
    Q_OBJECT

public:
    explicit AudioEngine(QObject *parent = nullptr);
    ~AudioEngine();

    bool initialize();

    void start();
    void stop();

    struct SequenceClipPlayback
    {
        const QVector<float>* samples = nullptr;

        std::int64_t startSample = 0;
        std::int64_t sourceStartSample = 0;
        std::int64_t sourceEndSample = 0;
    };
    void noteOn(int midiNote, int velocity);
    void noteOff(int midiNote);
    void noteOn(int trackId, int midiNote, int velocity);
    void noteOff(int trackId, int midiNote);

    QVector<float> renderClip(
        int trackId,
        const QVector<PlacedNote>& notes);
        
    bool shouldTrackParticipate(int trackId) const;

    void setTrackMuted(int trackId, bool muted);
    bool isTrackMuted(int trackId) const;

    void setTrackSoloed(int trackId, bool soloed);
    bool isTrackSoloed(int trackId) const;

    void startSequence(
        const QVector<SequenceClipPlayback>& clips
    );


    void stopSequence();

    bool isSequencePlaying() const;

    std::int64_t sequencePlaybackPosition() const;

    void setClippingThreshold(int trackId, float threshold);
    
    void setDistortionDrive(int trackId, float drive);

    void setDelayTime(int trackId, double time);

    void setDelayFeedback(int trackId, double feedback);

    void setDelayMix(int trackId, double mix);

    void setEQLowFrequency(int trackId, float frequency);

    void setEQLowGain(int trackId, float gain);

    void setEQMidFrequency(int trackId, float frequency);

    void setEQMidGain(int trackId, float gain);

    void setEQMidQ(int trackId, float q);

    void setEQHighFrequency(int trackId, float frequency);
    
    void setEQHighGain(int trackId, float gain);
    
    void setReverbRoomSize(int trackId, float roomSize);

    void setReverbDamping(int trackId, float damping);

    void setReverbWet(int trackId, float wet);

    void setReverbDecay(int trackId, float decay);

    void setSaturationDrive(int trackId, float drive);

    void setFlangerRate(int trackId, float rate);

    void setFlangerDepth(int trackId, float depth);

    void setFlangerFeedback(int trackId, float feedback);

    void setFlangerMix(int trackId, float mix);

    void setCompressionThreshold(int trackId, float threshold);

    void setCompressionRatio(int trackId, float ratio);

    void setCompressionAttack(int trackId, float attack);

    void setCompressionRelease(int trackId, float release);

    void setCompressionMakeupGain(int trackId, float gain);

    // Limiter
    void setLimiterThreshold(int trackId, float threshold);
    void setLimiterRelease(int trackId, float release);
    void setLimiterCeiling(int trackId, float ceiling);

    // Gate
    void setGateThreshold(int trackId, float threshold);
    void setGateAttack(int trackId, float attack);
    void setGateRelease(int trackId, float release);
    void setGateRange(int trackId, float range);

    // Chorus
    void setChorusRate(int trackId, float rate);
    void setChorusDepth(int trackId, float depth);
    void setChorusMix(int trackId, float mix);

    // Phaser
    void setPhaserRate(int trackId, float rate);
    void setPhaserDepth(int trackId, float depth);
    void setPhaserFeedback(int trackId, float feedback);
    void setPhaserMix(int trackId, float mix);
    
    // Oscillator and Voice

    // void setOscillatorFrequency(int trackId, float frequency);

    void setOscillatorVolume(int trackId,float volume);

    void setOscillatorWaveform(int trackId,Oscillator::Waveform waveform);

    void setVelocity(int trackId,float velocity);
    // ADSR
    void setAttack(int trackId, float seconds);
    void setDecay(int trackId, float seconds);
    void setSustain(int trackId, float level);
    void setRelease(int trackId, float seconds);
    void setPeakGain(int trackId, float level);
    
    void pitchBend(int channel, int value);

    void render(
        float *left,
        float *right,
        std::size_t numSamples
    );

    void setEffectEnabled(
        int trackId,
        AudioEffectType effectType,
        bool enabled
    );

    void setEffectOrder(
        int trackId,
        const QVector<AudioEffectType> &order
    );
    TrackProcessor* createTrack(int trackId);
    TrackProcessor* findTrackProcessor(int trackId);
    const TrackProcessor* findTrackProcessor(int trackId) const;
    double sampleRate() const;
    int blockSize() const;

private:
    double m_sampleRate = 48000.0;
    int m_blockSize = 256;
    struct TrackRoutingState
    {
        bool muted = false;
        bool soloed = false;
    };

    QVector<SequenceClipPlayback> m_sequenceClips;

    std::atomic<bool> m_sequencePlaying{false};
    std::atomic<std::int64_t> m_sequencePlaybackPosition{0};

    QHash<int, TrackRoutingState> m_trackRoutingStates;

    void renderSequence(
        float* left,
        float* right,
        std::size_t numSamples
    );
    QAudioFormat m_format;
    QAudioSink *m_audioSink = nullptr;
    QIODevice *m_audioDevice = nullptr;

    AudioOutputDevice *m_outputDevice = nullptr;

    std::vector<std::unique_ptr<TrackProcessor>> m_trackProcessors;

    bool m_running = false;
};