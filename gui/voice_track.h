#pragma once

#include <QWidget>
#include <QPoint>
#include <Vector>
#include "../synth/InstrumentParameters.h"
#include "../synth/SynthesizersSamples.h"
class QLabel;
class QPushButton;
class InstrumentSettings;
class AudioEngine;
class Piano;
class QMouseEvent;
struct PlacedNote;
class PianoRoll;

class VoiceTrack : public QWidget
{
    Q_OBJECT

public:
    explicit VoiceTrack(
    int trackId,
    const QString &trackName,
    AudioEngine *audioEngine,
    QWidget *parent = nullptr
);
    const InstrumentParameters& instrumentParameters() const;
    int trackId() const;
    QString trackName() const;
    const QVector<PlacedNote>& notes() const;
    double trackDuration() const;

signals:
    void pianoRollRequested();
    // Signals handled by Qt moc itself
    void instrumentRequested();
    void effectsRequested();
    void pianoRollBecameActive(PianoRoll* pianoRoll);
    void muteChanged(bool muted);
    void soloChanged(bool solo);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void onSampleInstrumentChanged(SynthesizersSamples sample);

private:
    void createUI();
    QPoint m_dragStartPosition;

    int m_trackId;
    QString m_trackName;

    AudioEngine *m_audioEngine;
    
    bool m_muted = false;
    bool m_solo = false;

    QLabel *m_nameLabel;

    QPushButton *m_pianoRollButton;
    QPushButton *m_instrumentButton;
    QPushButton *m_effectsButton;

    QPushButton *m_muteButton;
    QPushButton *m_soloButton;
    InstrumentSettings *m_instrumentSettings;
    InstrumentParameters m_instrumentParameters;
    Piano *m_piano ;
};