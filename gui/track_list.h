#pragma once

#include <QWidget>

class QHBoxLayout;
class QPushButton;
class AudioEngine;

class TrackList : public QWidget
{
    Q_OBJECT

public:
    explicit TrackList(AudioEngine * audioEngine, QWidget *parent = nullptr);

    void addVoiceTrack();

signals:
    void voiceTrackAdded(int trackId);

private:
    void createUI();
    
    AudioEngine *m_audioEngine;

    QHBoxLayout *m_trackLayout;
    QPushButton *m_addTrackButton;

    int m_nextTrackId = 1;
};