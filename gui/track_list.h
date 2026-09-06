#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>

class QHBoxLayout;
class QPushButton;
class AudioEngine;

class TrackList : public QWidget
{
    Q_OBJECT

public:
    explicit TrackList(AudioEngine * audioEngine, QWidget *parent = nullptr);

    void addVoiceTrack();
    QVector<QPair<int, QString>> availableTracks() const;
signals:
    
    void tracksChanged(const QVector<QPair<int, QString>> &tracks);

private:
    void createUI();
    
    AudioEngine *m_audioEngine;
    QVector<QPair<int, QString>> m_tracks;
    QHBoxLayout *m_trackLayout;
    QPushButton *m_addTrackButton;

    int m_nextTrackId = 1;
};