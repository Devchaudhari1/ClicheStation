#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QLabel;
class TimeliineView;
class Sequence;
class AudioEngine;
class TrackList;
#include "TimelineView.h"
class Playlist : public QWidget
{
    Q_OBJECT

public:
    explicit Playlist(QWidget *parent = nullptr);
    // void layoutSequences();
public slots:
    void addSequence(
        AudioEngine *audioEngine,
        TrackList *trackList);
    void setAvailableTracks(
        const QVector<QPair<int, QString>> &tracks
    );
private:
    void createUI();

    TimelineView *m_timelineView;

    QVector<Sequence *> m_sequences;
    int m_nextSequenceId = 1;

    const int sequenceHeaderHeight = 60;
    const int layerHeight = 120;

    QVector<QWidget*> m_leftSequenceContainers;
    QVector<QWidget*> m_rightSequenceContainers;

    QVector<QVBoxLayout*> m_leftSequenceLayouts;
    QVector<QVBoxLayout*> m_rightSequenceLayouts;

    QVector<QPair<int, QString>> m_availableTracks;
};