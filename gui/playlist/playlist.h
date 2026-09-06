#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
#include <QHash>

class QVBoxLayout;
class QPushButton;
class Sequence;
class QScrollArea;
class QWidget;
class TimeRuler;
class QScrollBar;

class Playlist : public QWidget
{
    Q_OBJECT

public:
    explicit Playlist(QWidget *parent = nullptr);

    Sequence *addSequence();

    void setAvailableTracks(
        const QVector<QPair<int, QString>> &tracks
    );

private:

    QVector<QPair<int, QString>> m_availableTracks;

    QVBoxLayout *m_sequenceLayout;

    QWidget *m_timelineBody;
    QWidget *m_leftColumn;
    QWidget *m_leftContent;

    QScrollArea *m_scrollArea;
    QWidget *m_timelineContent;
    int m_leftContentHeight = 0;

    QVBoxLayout *m_leftLayout;

    QScrollBar *m_horizontalScrollBar;

    QHash<Sequence*, QWidget*> m_sequenceLeftContainers;

    TimeRuler *m_timeRuler;

    Sequence *m_lastSequence = nullptr;

    int m_nextSequenceId = 1;
};