#pragma once

#include <QObject>
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <vector>
#include <memory>
#include <QString>
#include <QVector>
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

    void noteOn(int midiNote, int velocity);
    void noteOff(int midiNote);
    void noteOn(int trackId, int midiNote, int velocity);
    void noteOff(int trackId, int midiNote);

    void setDistortionDrive(int trackId, float drive);
    
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
    double sampleRate() const;
    int blockSize() const;

private:
    double m_sampleRate = 48000.0;
    int m_blockSize = 256;

    QAudioFormat m_format;
    QAudioSink *m_audioSink = nullptr;
    QIODevice *m_audioDevice = nullptr;

    AudioOutputDevice *m_outputDevice = nullptr;

    std::vector<std::unique_ptr<TrackProcessor>> m_trackProcessors;

    bool m_running = false;
};