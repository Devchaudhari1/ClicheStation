#pragma once

#include <QWidget>

class PianoRoll;
class PianoKeyboard;
class AudioEngine;

class Piano : public QWidget
{
    Q_OBJECT

public:
    explicit Piano(AudioEngine *audioEngine,
               QWidget *parent = nullptr);
    ~Piano();

private:
    void createUI();

    PianoRoll *pianoRoll;
    PianoKeyboard *pianoKeyboard;
    AudioEngine *audioEngine;
};