#include "voice_track.h"
#include "../audio/AudioEngine.h"
#include "instrument_settings.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

VoiceTrack::VoiceTrack(
    int trackId,
    const QString &trackName,
    AudioEngine *audioEngine,
    QWidget *parent
)
    : QWidget(parent),
      m_trackId(trackId),
      m_trackName(trackName),
      m_audioEngine(audioEngine),
      m_instrumentSettings(nullptr)
{
    setFixedSize(180, 220);

    createUI();
}

const InstrumentParameters&
VoiceTrack::instrumentParameters() const
{
    return m_instrumentParameters;
}

void VoiceTrack::createUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Track name
    m_nameLabel = new QLabel(m_trackName, this);
    m_nameLabel->setAlignment(Qt::AlignCenter);

    // Editor buttons
    m_pianoRollButton = new QPushButton("Piano Roll", this);
    m_instrumentButton = new QPushButton("Instrument", this);
    m_effectsButton = new QPushButton("Effects", this);

    // Mute / Solo
    m_muteButton = new QPushButton("Mute", this);
    m_soloButton = new QPushButton("Solo", this);

    m_muteButton->setCheckable(true);
    m_soloButton->setCheckable(true);

    // Add widgets
    mainLayout->addWidget(m_nameLabel);

    mainLayout->addSpacing(10);

    mainLayout->addWidget(m_pianoRollButton);
    mainLayout->addWidget(m_instrumentButton);
    mainLayout->addWidget(m_effectsButton);

    mainLayout->addStretch();

    QHBoxLayout *mixLayout = new QHBoxLayout;

    mixLayout->addWidget(m_muteButton);
    mixLayout->addWidget(m_soloButton);

    mainLayout->addLayout(mixLayout);

    // Button signals
    connect(
        m_pianoRollButton,
        &QPushButton::clicked,
        this,
        &VoiceTrack::pianoRollRequested
    );

   connect(
    m_instrumentButton,
    &QPushButton::clicked,
    this,
    [this]()
    {
        if (!m_instrumentSettings)
        {
            m_instrumentSettings =
                new InstrumentSettings(nullptr);

            connect(
                m_instrumentSettings,
                &InstrumentSettings::previewNotePressed,
                this,
                [this](int midiNote)
                {
                    if (m_audioEngine)
                        m_audioEngine->noteOn(midiNote, 100);
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::previewNoteReleased,
                this,
                [this](int midiNote)
                {
                    if (m_audioEngine)
                        m_audioEngine->noteOff(midiNote);
                }
            );
        }

        m_instrumentSettings->show();
        m_instrumentSettings->raise();
        m_instrumentSettings->activateWindow();
        }
    );

    connect(
        m_effectsButton,
        &QPushButton::clicked,
        this,
        &VoiceTrack::effectsRequested
    );

    connect(
        m_muteButton,
        &QPushButton::toggled,
        this,
        [this](bool checked)
        {
            m_muted = checked;
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
            emit soloChanged(m_solo);
        }
    );
}

int VoiceTrack::trackId() const
{
    return m_trackId;
}

QString VoiceTrack::trackName() const
{
    return m_trackName;
}