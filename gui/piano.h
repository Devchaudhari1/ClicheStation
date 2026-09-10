#pragma once

#include <QWidget>
#include <QVector>

class PianoRoll;
class PianoKeyboard;
class AudioEngine;
struct PlacedNote;

class Piano : public QWidget
{
    Q_OBJECT

public:
    explicit Piano(
    int trackId,
    AudioEngine *audioEngine,
    QWidget *parent = nullptr
);
    ~Piano();
    const QVector<PlacedNote>& notes() const;

private:
    void createUI();
    
    int trackId;
    PianoRoll *pianoRoll;
    PianoKeyboard *pianoKeyboard;
    AudioEngine *audioEngine;
};