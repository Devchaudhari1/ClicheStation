#include "Sequence.h"
#include "Layer.h"
#include "Clip.h"
#include "../voice_track.h"
#include "../track_list.h"
#include "../../audio/AudioEngine.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>

Sequence::Sequence(
    int sequenceId,
    AudioEngine *audioEngine,
    QWidget *parent
)
    : QWidget(parent),
      m_sequenceId(sequenceId),
      m_audioEngine(audioEngine),
      m_trackList(nullptr),
      m_header(nullptr),
      m_expandButton(nullptr),
      m_sequenceLabel(nullptr),
      m_addLayerButton(nullptr),
      m_playButton(nullptr)
{
    // -------------------------------------------------
    // Sequence header
    // -------------------------------------------------

    m_header =
        new QWidget(this);

    m_header->setObjectName(
        "sequenceHeader"
    );

    m_header->setFixedHeight(44);

    QHBoxLayout *headerLayout =
        new QHBoxLayout(m_header);

    headerLayout->setContentsMargins(
        10, 5, 10, 5
    );

    headerLayout->setSpacing(6);

    headerLayout->setAlignment(Qt::AlignTop);
    // -------------------------------------------------
    // Expand / collapse
    // -------------------------------------------------

    m_expandButton =
        new QPushButton("▼", m_header);

    m_expandButton->setObjectName(
        "expandButton"
    );

    m_expandButton->setFixedSize(
        28,
        20
    );

    headerLayout->addWidget(
        m_expandButton
    );


    // -------------------------------------------------
    // Sequence label
    // -------------------------------------------------

    m_sequenceLabel =
        new QLabel(
            QString("Sequence %1")
                .arg(m_sequenceId),
            m_header
        );

    m_sequenceLabel->setObjectName(
        "sequenceLabel"
    );

    headerLayout->addWidget(
        m_sequenceLabel
    );


    // -------------------------------------------------
    // Push controls to the right
    // -------------------------------------------------

    headerLayout->addStretch();


    // -------------------------------------------------
    // Play
    // -------------------------------------------------

    m_playButton =
        new QPushButton("▶", m_header);

    m_playButton->setObjectName(
        "playButton"
    );

    m_playButton->setFixedSize(
        34,
        20
    );

    headerLayout->addWidget(
        m_playButton
    );


    // -------------------------------------------------
    // Add Layer
    // -------------------------------------------------

    m_addLayerButton =
        new QPushButton(
            "+",
            m_header
        );

    m_addLayerButton->setObjectName(
        "addLayerButton"
    );

    m_addLayerButton->setFixedHeight(20);

    headerLayout->addWidget(
        m_addLayerButton
    );

    // -------------------------------------------------
    // Connections
    // -------------------------------------------------

    connect(
        m_expandButton,
        &QPushButton::clicked,
        this,
        &Sequence::toggleExpanded
    );

    connect(
        m_addLayerButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            addLayer();
        }
    );

    connect(
        m_playButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            play();
        }
    );


    // -------------------------------------------------
    // Start with one layer
    // -------------------------------------------------

    addLayer();
}

int Sequence::sequenceId() const
{
    return m_sequenceId;
}

void Sequence::toggleExpanded()
{
    m_expanded = !m_expanded;

    m_expandButton->setText(
        m_expanded ? "▼" : "▶"
    );

    emit expandedChanged(m_expanded);
}

QVector<AudioEngine::SequenceClipPlayback>
Sequence::buildPlaybackClips() const
{
    QVector<AudioEngine::SequenceClipPlayback> result;

    const double sampleRate =
        m_audioEngine->sampleRate();

    for (Layer* layer : m_layers)
    {
        if (!layer)
            continue;

        for (Clip* clip : layer->clips())
        {
            if (!clip)
                continue;

            AudioEngine::SequenceClipPlayback playback;

            playback.samples =
                &clip->samples();

            playback.startSample =
                static_cast<std::int64_t>(
                    clip->startTime() * sampleRate
                );

            playback.sourceStartSample =
                static_cast<std::int64_t>(
                    clip->sourceStart() * sampleRate
                );

            playback.sourceEndSample =
                static_cast<std::int64_t>(
                    clip->sourceEnd() * sampleRate
                );

            result.append(playback);
        }
    }

    return result;
}

void Sequence::play()
{
    if (!m_audioEngine)
        return;

    const auto clips =
        buildPlaybackClips();

    if (clips.isEmpty())
        return;

    m_audioEngine->startSequence(clips);
}

void Sequence::stop()
{
    if (!m_audioEngine)
        return;

    m_audioEngine->stopSequence();
}

Layer *Sequence::addLayer()
{
    Layer *layer =
        new Layer(nullptr);

    connect(
        layer,
        &Layer::timelineWidthRequired,
        this,
        &Sequence::timelineWidthRequired
    );
    
    connect(
        layer,
        &Layer::clipRequested,
        this,
        [this, layer]()
        {
            QMenu menu;

            for (const auto &track : m_availableTracks)
            {
                QAction *action =
                    menu.addAction(track.second);

                action->setData(track.first);
            }

            if (menu.isEmpty())
                return;

            QAction *selected =
                menu.exec(
                    layer->mapToGlobal(
                        QPoint(
                            layer->width(),
                            layer->height()
                        )
                    )
                );

            if (!selected)
                return;

            int trackId = selected->data().toInt();
            QString trackName = selected->text();

            VoiceTrack *voiceTrack =
                m_trackList->findVoiceTrack(trackId);

            if (!voiceTrack)
                return;

            const QVector<PlacedNote>& notes =
                voiceTrack->notes();
            double startTime=0.0;
            double duration =
                voiceTrack->trackDuration();

            const auto &clips =
                layer->clips();

            if (!clips.isEmpty())
            {
                Clip *lastClip =
                    clips.last();

                startTime =
                    lastClip->startTime()
                    + lastClip->duration();
            }
            // Render the PianoRoll notes into audio samples.
            QVector<float> samples =
                m_audioEngine->renderClip(
                    trackId,
                    notes
                );

            if (samples.isEmpty())
                return;

            layer->addClip(
                trackId,
                trackName,
                startTime,
                duration,
                samples
            );
        }
    );

    connect(
        layer,
        &Layer::trackDropped,
        this,
        [this, layer](
            int trackId,
            double startTime
        )
        {
            qDebug() << "TRACK DROPPED:" << trackId;
            qDebug() << "AudioEngine:" << m_audioEngine;
            qDebug() << "TrackList:" << m_trackList;

            if (!m_audioEngine)
            {
                qDebug() << "ERROR: m_audioEngine is null";
                return;
            }

            if (!m_trackList)
            {
                qDebug() << "ERROR: m_trackList is null";
                return;
            }

            VoiceTrack *voiceTrack =
                m_trackList->findVoiceTrack(trackId);

            qDebug() << "VoiceTrack:" << voiceTrack;

            if (!voiceTrack)
            {
                qDebug() << "ERROR: VoiceTrack not found";
                return;
            }

            const QVector<PlacedNote>& notes =
                voiceTrack->notes();

            qDebug() << "Notes:" << notes.size();

            double duration =
                voiceTrack->trackDuration();

            qDebug() << "Duration:" << duration;

            QVector<float> samples =
                m_audioEngine->renderClip(
                    trackId,
                    notes
                );

            qDebug() << "Samples:" << samples.size();

            if (samples.isEmpty())
                return;

            layer->addClip(
                trackId,
                voiceTrack->trackName(),
                startTime,
                duration,
                samples
            );
        }
    );

    m_layers.append(layer);

    emit layerAdded();

    return layer;
}

void Sequence::setTrackList(TrackList *trackList)
{
    m_trackList = trackList;
}

void Sequence::setAvailableTracks(
    const QVector<QPair<int, QString>> &tracks
)
{
    m_availableTracks = tracks;
}

QWidget *Sequence::headerWidget() const
{
    return m_header;
}


const QVector<Layer *> &Sequence::layers() const
{
    return m_layers;
}

bool Sequence::isExpanded() const
{
    return m_expanded;
}