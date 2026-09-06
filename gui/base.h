#ifndef BASE_H
#define BASE_H

#include <QWidget>
#include <QMainWindow>
#include <QVector>

class AudioEngine;
class VoiceTrack;
class Piano;
class MonitorWindow;
class DrumPad;
class Playlist;
class Base : public QMainWindow
{
public:
    explicit Base(QWidget *parent = nullptr);
    ~Base();

    void createUI();
    void playStartupSound();
private:
    Playlist *m_playlist;
    Piano *pianoWindow = nullptr;
    AudioEngine *m_audioEngine = nullptr;
    MonitorWindow *monitorWindow = nullptr;
    DrumPad *drumPadWindow = nullptr;
    QVector<VoiceTrack*> voiceTracks;
};

#endif // BASE_H