#include "audio_track.h"

#include "./playlist/Clip.h"
#include "../audio/AudioEngine.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

AudioTrack::AudioTrack(
    int trackId,
    const QString& trackName,
    AudioEngine* audioEngine,
    QWidget* parent
)
    : QWidget(parent)
    , m_trackId(trackId)
    , m_trackName(trackName)
    , m_audioEngine(audioEngine)
{
    setFixedSize(220, 180);

    createUI();
}

void AudioTrack::createUI()
{
    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    m_nameLabel = new QLabel(
        m_trackName,
        this
    );

    m_nameLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(m_nameLabel);

    m_clip = new Clip(
        m_trackId,
        m_trackName,
        0.0,
        0.05,
        this
    );

    mainLayout->addWidget(
        m_clip,
        1
    );

    auto* mixLayout = new QHBoxLayout;

    m_muteButton = new QPushButton(
        "Mute",
        this
    );

    m_soloButton = new QPushButton(
        "Solo",
        this
    );

    m_muteButton->setCheckable(true);
    m_soloButton->setCheckable(true);

    mixLayout->addWidget(m_muteButton);
    mixLayout->addWidget(m_soloButton);

    mainLayout->addLayout(mixLayout);

    connect(
        m_muteButton,
        &QPushButton::toggled,
        this,
        [this](bool checked)
        {
            m_muted = checked;

            if (m_audioEngine)
            {
                m_audioEngine->setTrackMuted(
                    m_trackId,
                    checked
                );
            }

            emit muteChanged(m_muted);
        }
    );

    connect(
        m_soloButton,
        &QPushButton::toggled,
        this,
        [this](bool checked)
        {
            m_solo = checked;

            if (m_audioEngine)
            {
                m_audioEngine->setTrackSoloed(
                    m_trackId,
                    checked
                );
            }

            emit soloChanged(m_solo);
        }
    );
}

int AudioTrack::trackId() const
{
    return m_trackId;
}

QString AudioTrack::trackName() const
{
    return m_trackName;
}

Clip* AudioTrack::clip() const
{
    return m_clip;
}

void AudioTrack::setAudio(
    const QVector<float>& samples,
    double duration
)
{
    if (!m_clip)
        return;

    m_clip->setDuration(duration);

    m_clip->setSourceRange(
        0.0,
        duration
    );

    m_clip->setSamples(
        samples,
        m_clip->width()
    );
}