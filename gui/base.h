#pragma once

#include <QMainWIndow>
#include <QVector>

class AudioEngine;
class VoiceTrack;
class Piano;
class MonitorWindow;
class DrumPad;
class Playlist;
class TrackList;

class Base : public QMainWindow
{
public:
    explicit Base(QWidget *parent = nullptr);
    ~Base();

    void createUI();
    void playStartupSound();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Playlist *m_playlist;

    TrackList *m_trackList = nullptr;

    Piano *pianoWindow = nullptr;
    AudioEngine *m_audioEngine = nullptr;
    MonitorWindow *monitorWindow = nullptr;
    DrumPad *drumPadWindow = nullptr;

    QVector<VoiceTrack*> voiceTracks;
};