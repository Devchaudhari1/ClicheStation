#include "voice_track.h"
#include "../audio/AudioEngine.h"
#include "instrument_settings.h"
#include "piano.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QByteArray>
#include <QApplication>
#include <QDebug>
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
      m_instrumentSettings(nullptr),
      m_piano(nullptr)
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
    [this]()
    {
        if (!m_piano)
        {
            m_piano = new Piano(m_trackId, m_audioEngine, nullptr);
            m_piano->setWindowTitle(m_trackName + " - Piano");
        }

        m_piano->show();
        m_piano->raise();
        m_piano->activateWindow();
    });

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

            // Preview note
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

            // Distortion
            connect(
                m_instrumentSettings,
                &InstrumentSettings::distortionDriveChanged,
                this,
                [this](float drive)
                {
                    if (m_audioEngine)
                        m_audioEngine->setDistortionDrive(
                            m_trackId,
                            drive
                        );
                }
            );

            // Effect enabled / bypass
            connect(
                m_instrumentSettings,
                &InstrumentSettings::effectEnabledChanged,
                this,
                [this](AudioEffectType effectType, bool enabled)
                {
                    qDebug() << "VoiceTrack received effect signal:"
                             << "type =" << static_cast<int>(effectType)
                             << "enabled =" << enabled;

                    if (!m_audioEngine)
                    {
                        qDebug() << "VoiceTrack: m_audioEngine is NULL";
                        return;
                    }

                    m_audioEngine->setEffectEnabled(
                        m_trackId,
                        effectType,
                        enabled
                    );
                }
            );

            // Effect order
            connect(
                m_instrumentSettings,
                &InstrumentSettings::effectOrderChanged,
                this,
                [this](const QVector<AudioEffectType> &order)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setEffectOrder(
                        m_trackId,
                        order
                    );
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

void VoiceTrack::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - m_dragStartPosition).manhattanLength()
        < QApplication::startDragDistance())
    {
        return;
    }

    qDebug() << "STARTING DRAG FOR TRACK" << m_trackId;

    QMimeData *mimeData = new QMimeData;

    mimeData->setData(
        "application/x-clichestation-track",
        QByteArray::number(m_trackId)
        );

    QDrag *drag = new QDrag(this);

    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction);
}

void VoiceTrack::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->pos();
    }

    QWidget::mousePressEvent(event);
}