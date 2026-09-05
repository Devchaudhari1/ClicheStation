#include "track_list.h"
#include "voice_track.h"

#include <QHBoxLayout>
#include <QPushButton>
#include "../audio/AudioEngine.h"
TrackList::TrackList(AudioEngine *audioEngine, QWidget *parent)
    : QWidget(parent),
      m_audioEngine(audioEngine),
      m_trackLayout(nullptr),
      m_addTrackButton(nullptr)
{
    createUI();
}

void TrackList::createUI()
{
    m_trackLayout = new QHBoxLayout(this);

    m_addTrackButton = new QPushButton("+ Add Voice Track", this);

    m_trackLayout->addWidget(m_addTrackButton);
    m_trackLayout->addStretch();

    connect(m_addTrackButton, &QPushButton::clicked,
            this, &TrackList::addVoiceTrack);
}

void TrackList::addVoiceTrack()
{
    int trackId = m_nextTrackId++;

    QString trackName =
        QString("Voice Track %1").arg(trackId);

    VoiceTrack *track =
    new VoiceTrack(trackId, trackName, m_audioEngine, this);

    int insertPosition = m_trackLayout->count() - 1;

    m_trackLayout->insertWidget(insertPosition, track);

    emit voiceTrackAdded(trackId);
}