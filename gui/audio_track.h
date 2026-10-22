#pragma once

#include <QWidget>
#include <QVector>
#include <QString>

class QLabel;
class QPushButton;
class AudioEngine;
class Clip;

class AudioTrack : public QWidget
{
    Q_OBJECT

public:
    explicit AudioTrack(
        int trackId,
        const QString& trackName,
        AudioEngine* audioEngine,
        QWidget* parent = nullptr
    );

    int trackId() const;
    QString trackName() const;

    Clip* clip() const;

    void setAudio(
        const QVector<float>& samples,
        double duration
    );
    
signals:
    void muteChanged(bool muted);
    void soloChanged(bool solo);

private:
    void createUI();

    int m_trackId;
    QString m_trackName;

    AudioEngine* m_audioEngine;

    bool m_muted = false;
    bool m_solo = false;

    Clip* m_clip = nullptr;

    QLabel* m_nameLabel = nullptr;
    QPushButton* m_muteButton = nullptr;
    QPushButton* m_soloButton = nullptr;
};