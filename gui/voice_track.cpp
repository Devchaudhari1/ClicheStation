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
            // Oscillator and Voice
            connect(
                m_instrumentSettings,
                &InstrumentSettings::oscillatorTypeChanged,
                this,
                [this](Oscillator::Waveform waveform)
                {
                    if(m_audioEngine)
                    m_audioEngine->setOscillatorWaveform(m_trackId,waveform);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::volumeChanged,
                this,
                [this](double volume)
                {
                    if(m_audioEngine)
                    m_audioEngine->setOscillatorVolume(m_trackId,static_cast<float>(volume));
                });  

            connect(
                m_instrumentSettings,
                &InstrumentSettings::velocityChanged,
                this,
                [this](double velocity)
                {
                    if(m_audioEngine)
                    m_audioEngine->setVelocity(m_trackId,static_cast<float>(velocity));
                });
            //ADSR
            connect(
                m_instrumentSettings,
                &InstrumentSettings::attackChanged,
                this,
                [this](float value)
                {
                    m_audioEngine->setAttack(
                        m_trackId,
                        value
                    );
                });
            connect(
                m_instrumentSettings,
                &InstrumentSettings::decayChanged,
                this,
                [this](float value)
                {
                    m_audioEngine->setDecay(
                        m_trackId,
                        value
                    );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::sustainChanged,
                this,
                [this](float value)
                {
                    m_audioEngine->setSustain(
                        m_trackId,
                        value
                    );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::releaseChanged,
                this,
                [this](float value)
                {
                    m_audioEngine->setRelease(
                        m_trackId,
                        value
                    );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::peakGainChanged,
                this,
                [this](float value)
                {
                    m_audioEngine->setPeakGain(
                        m_trackId,
                        value
                    );
                }
            );
            // Clipping
            connect(
                m_instrumentSettings,
                &InstrumentSettings::clippingThresholdChanged,
                this,
                [this](float threshold)
                {
                    if (m_audioEngine)
                        m_audioEngine->setClippingThreshold(m_trackId, threshold);
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
            // Saturation
            connect(
                m_instrumentSettings,
                &InstrumentSettings::saturationDriveChanged,
                this,
                [this](float drive)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setSaturationDrive(
                        m_trackId,
                        drive
                    );
                }
            );
            // Flanger
            connect(
                m_instrumentSettings,
                &InstrumentSettings::flangerRateChanged,
                this,
                [this](float rate)
                {
                    m_audioEngine->setFlangerRate(
                        m_trackId,
                        rate);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::flangerDepthChanged,
                this,
                [this](float depth)
                {
                    m_audioEngine->setFlangerDepth(
                        m_trackId,
                        depth);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::flangerFeedbackChanged,
                this,
                [this](float feedback)
                {
                    m_audioEngine->setFlangerFeedback(
                        m_trackId,
                        feedback);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::flangerMixChanged,
                this,
                [this](float mix)
                {
                    m_audioEngine->setFlangerMix(
                        m_trackId,
                        mix);
                });
            //Limiter
            connect(
                m_instrumentSettings,
                &InstrumentSettings::limiterThresholdChanged,
                this,
                [this](float threshold)
                {
                    if (m_audioEngine)
                        m_audioEngine->setLimiterThreshold(
                            m_trackId,
                            threshold
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::limiterReleaseChanged,
                this,
                [this](float release)
                {
                    if (m_audioEngine)
                        m_audioEngine->setLimiterRelease(
                            m_trackId,
                            release
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::limiterCeilingChanged,
                this,
                [this](float ceiling)
                {
                    if (m_audioEngine)
                        m_audioEngine->setLimiterCeiling(
                            m_trackId,
                            ceiling
                        );
                }
            );

            // Gate
            connect(
                m_instrumentSettings,
                &InstrumentSettings::gateThresholdChanged,
                this,
                [this](float threshold)
                {
                    if (m_audioEngine)
                        m_audioEngine->setGateThreshold(
                            m_trackId,
                            threshold
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::gateAttackChanged,
                this,
                [this](float attack)
                {
                    if (m_audioEngine)
                        m_audioEngine->setGateAttack(
                            m_trackId,
                            attack
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::gateReleaseChanged,
                this,
                [this](float release)
                {
                    if (m_audioEngine)
                        m_audioEngine->setGateRelease(
                            m_trackId,
                            release
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::gateRangeChanged,
                this,
                [this](float range)
                {
                    if (m_audioEngine)
                        m_audioEngine->setGateRange(
                            m_trackId,
                            range
                        );
                }
            );
            // Chorus
            connect(
                m_instrumentSettings,
                &InstrumentSettings::chorusRateChanged,
                this,
                [this](float rate)
                {
                    if (m_audioEngine)
                        m_audioEngine->setChorusRate(
                            m_trackId,
                            rate
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::chorusDepthChanged,
                this,
                [this](float depth)
                {
                    if (m_audioEngine)
                        m_audioEngine->setChorusDepth(
                            m_trackId,
                            depth
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::chorusMixChanged,
                this,
                [this](float mix)
                {
                    if (m_audioEngine)
                        m_audioEngine->setChorusMix(
                            m_trackId,
                            mix
                        );
                }
            );
            //Phaser
            connect(
                m_instrumentSettings,
                &InstrumentSettings::phaserRateChanged,
                this,
                [this](float rate)
                {
                    if (m_audioEngine)
                        m_audioEngine->setPhaserRate(
                            m_trackId,
                            rate
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::phaserDepthChanged,
                this,
                [this](float depth)
                {
                    if (m_audioEngine)
                        m_audioEngine->setPhaserDepth(
                            m_trackId,
                            depth
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::phaserFeedbackChanged,
                this,
                [this](float feedback)
                {
                    if (m_audioEngine)
                        m_audioEngine->setPhaserFeedback(
                            m_trackId,
                            feedback
                        );
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::phaserMixChanged,
                this,
                [this](float mix)
                {
                    if (m_audioEngine)
                        m_audioEngine->setPhaserMix(
                            m_trackId,
                            mix
                        );
                }
            );
            // Compression
            connect(
                m_instrumentSettings,
                &InstrumentSettings::compressionThresholdChanged,
                this,
                [this](float threshold)
                {
                    m_audioEngine->setCompressionThreshold(
                        m_trackId,
                        threshold);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::compressionRatioChanged,
                this,
                [this](float ratio)
                {
                    m_audioEngine->setCompressionRatio(
                        m_trackId,
                        ratio);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::compressionAttackChanged,
                this,
                [this](float attack)
                {
                    m_audioEngine->setCompressionAttack(
                        m_trackId,
                        attack);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::compressionReleaseChanged,
                this,
                [this](float release)
                {
                    m_audioEngine->setCompressionRelease(
                        m_trackId,
                        release);
                });

            connect(
                m_instrumentSettings,
                &InstrumentSettings::compressionMakeupGainChanged,
                this,
                [this](float gain)
                {
                    m_audioEngine->setCompressionMakeupGain(
                        m_trackId,
                        gain);
                });
            
            // EQ
            connect(
                m_instrumentSettings,
                &InstrumentSettings::eqLowFrequencyChanged,
                this,
                [this](float frequency)
                {
                    if (m_audioEngine)
                        m_audioEngine->setEQLowFrequency(m_trackId, frequency);
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::eqLowGainChanged,
                this,
                [this](float gain)
                {
                    if (m_audioEngine)
                        m_audioEngine->setEQLowGain(m_trackId, gain);
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::eqMidFrequencyChanged,
                this,
                [this](float frequency)
                {
                    if (m_audioEngine)
                        m_audioEngine->setEQMidFrequency(m_trackId, frequency);
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::eqMidGainChanged,
                this,
                [this](float gain)
                {
                    if (m_audioEngine)
                        m_audioEngine->setEQMidGain(m_trackId, gain);
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::eqMidQChanged,
                this,
                [this](float q)
                {
                    if (m_audioEngine)
                        m_audioEngine->setEQMidQ(m_trackId, q);
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::eqHighFrequencyChanged,
                this,
                [this](float frequency)
                {
                    if (m_audioEngine)
                        m_audioEngine->setEQHighFrequency(m_trackId, frequency);
                }
            );

            connect(
                m_instrumentSettings,
                &InstrumentSettings::eqHighGainChanged,
                this,
                [this](float gain)
                {
                    if (m_audioEngine)
                        m_audioEngine->setEQHighGain(m_trackId, gain);
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
            // delayTime Changed
            connect(
                m_instrumentSettings,
                &InstrumentSettings::delayTimeChanged,
                this,
                [this](double time)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setDelayTime(
                        m_trackId,
                        time
                    );
                }
            );
            // delayfeedback Changed
            connect(
                m_instrumentSettings,
                &InstrumentSettings::delayFeedbackChanged,
                this,
                [this](double feedback)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setDelayFeedback(
                        m_trackId,
                        feedback
                    );
                }
            );
            //delayMixChanged
            connect(
                m_instrumentSettings,
                &InstrumentSettings::delayMixChanged,
                this,
                [this](double mix)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setDelayMix(
                        m_trackId,
                        mix
                    );
                }
            );
            //reverbRoomSizeChanged
            connect(
                m_instrumentSettings,
                &InstrumentSettings::reverbRoomSizeChanged,
                this,
                [this](float roomSize)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setReverbRoomSize(
                        m_trackId,
                        roomSize
                    );
                }
            );
            //reverbDampingChanged
            connect(
                m_instrumentSettings,
                &InstrumentSettings::reverbDampingChanged,
                this,
                [this](float damping)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setReverbDamping(
                        m_trackId,
                        damping
                    );
                }
            );
            //reverbWetChanged
            connect(
                m_instrumentSettings,
                &InstrumentSettings::reverbWetChanged,
                this,
                [this](float wet)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setReverbWet(
                        m_trackId,
                        wet
                    );
                }
            );
            //reverbDecayChanged
            connect(
                m_instrumentSettings,
                &InstrumentSettings::reverbDecayChanged,
                this,
                [this](float decay)
                {
                    if (!m_audioEngine)
                        return;

                    m_audioEngine->setReverbDecay(
                        m_trackId,
                        decay
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