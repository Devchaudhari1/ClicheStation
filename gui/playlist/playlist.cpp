#include "Playlist.h"
#include "TimeRuler.h"
#include "Sequence.h"
#include "ClipArea.h"
#include "Layer.h"
#include "../track_list.h"
#include "../../audio/AudioEngine.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMenu>
#include <QAction>

void Playlist::setAvailableTracks(
    const QVector<QPair<int, QString>> &tracks
)
{
    m_availableTracks = tracks;
    for (Sequence *sequence : m_sequences)
    {
        sequence->setAvailableTracks(m_availableTracks);
    }
}

void Playlist::addSequence(
    AudioEngine *audioEngine,
    TrackList *trackList
)
{
    int sequenceId = m_nextSequenceId++;

    Sequence *sequence =
        new Sequence(
            sequenceId,
            audioEngine,
            this
        );

    sequence->setTrackList(trackList);
    sequence->setAvailableTracks(m_availableTracks);

    m_sequences.append(sequence);

    connect(
        sequence,
        &Sequence::timelineWidthRequired,
        m_timelineView,
        &TimelineView::ensureWidthForTime
    );

    m_timelineView->addSequence(sequence);
}


Playlist::Playlist(QWidget *parent)
    : QWidget(parent)
{
    createUI();
   
}

void Playlist::createUI()
{
    QVBoxLayout *playlistLayout =
        new QVBoxLayout(this);

    playlistLayout->setContentsMargins(
        0, 0, 0, 0
    );

    playlistLayout->setSpacing(0);

    m_timelineView =
        new TimelineView(this);

    playlistLayout->addWidget(
        m_timelineView,
        1
    );
}