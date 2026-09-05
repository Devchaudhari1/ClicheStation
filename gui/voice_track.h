#pragma once

#include <QWidget>
#include "../synth/InstrumentParameters.h"
class QLabel;
class QPushButton;
class InstrumentSettings;
class AudioEngine;

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

signals:
    void pianoRollRequested();
    // Signals handled by Qt moc itself
    void instrumentRequested();
    void effectsRequested();

    void muteChanged(bool muted);
    void soloChanged(bool solo);

private:
    void createUI();

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
};