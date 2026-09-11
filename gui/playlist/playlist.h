#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QLabel;
class TimeRuler;
class Sequence;
class AudioEngine;
class TrackList;

class Playlist : public QWidget
{
    Q_OBJECT

public:
    explicit Playlist(QWidget *parent = nullptr);
    void layoutSequences();
public slots:
    void addSequence(
        AudioEngine *audioEngine,
        TrackList *trackList);
    void setAvailableTracks(
        const QVector<QPair<int, QString>> &tracks
    );
private:
    void createUI();
    void ensureTimelineWidthForTime(double endTime);
    // Main components
    TimeRuler *m_timeRuler;
    QWidget *m_timelineView;

    // Left side
    QScrollArea *m_leftPane;
    QWidget *m_leftContent;

    // Right side
    QScrollArea *m_rightPane;
    QWidget *m_rightContent;

    // Sequences
    QVector<Sequence *> m_sequences;
    int m_nextSequenceId = 1;
    const int sequenceHeaderHeight = 40;
    const int layerHeight = 80;
    QVector<QWidget*> m_leftSequenceContainers;
    QVector<QWidget*> m_rightSequenceContainers;

    QVector<QVBoxLayout*> m_leftSequenceLayouts;
    QVector<QVBoxLayout*> m_rightSequenceLayouts;

    // Tracks
    QVector<QPair<int, QString>> m_availableTracks;
    // Scroll synchronization
    bool m_syncingVerticalScroll = false;
};