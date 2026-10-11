#pragma once

#include <QWidget>

class PianoRoll;
class PianoKeyboard;

class VoiceTrackPianoEditor : public QWidget
{
    Q_OBJECT

public:
    explicit VoiceTrackPianoEditor(QWidget *parent = nullptr);

private:
    PianoRoll *m_pianoRoll;
    PianoKeyboard *m_pianoKeyboard;
};