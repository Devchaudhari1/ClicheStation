#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>

class QHBoxLayout;
class QPushButton;
class AudioEngine;
class QMouseEvent;

class TrackListHandle : public QWidget
{
    Q_OBJECT

public:
    explicit TrackListHandle(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent*);
signals:
    void dragDelta(int delta);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_dragging = false;
    int m_lastY = 0;
};

class TrackList : public QWidget
{
    Q_OBJECT

public:
    explicit TrackList(AudioEngine * audioEngine, QWidget *parent = nullptr);

    void addVoiceTrack();
    QVector<QPair<int, QString>> availableTracks() const;
    TrackListHandle *m_handle;

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