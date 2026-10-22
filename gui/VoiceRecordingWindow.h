#pragma once

#include <QWidget>
#include <QVector>

class AudioEngine;
class Clip;
class QPushButton;
class RecordingTimeRuler;

class VoiceRecordingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VoiceRecordingWindow(
        AudioEngine* audioEngine,
        QWidget* parent = nullptr
    );

private:
    void createUI();
    void loadRecording();

    AudioEngine* m_audioEngine = nullptr;

    QVector<float> m_monoSamples;
    QWidget* m_timelineWidget = nullptr;
    RecordingTimeRuler* m_timeRuler = nullptr;
    Clip* m_clip = nullptr;
    QPushButton* m_closeButton = nullptr;
    QPushButton* m_playButton = nullptr;
    QPushButton* m_stopButton = nullptr;
};