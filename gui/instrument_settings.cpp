#include "instrument_settings.h"
#include "pianoKeyboard.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>

InstrumentSettings::InstrumentSettings(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle("Instrument Settings");
    resize(450, 650);

    createUI();
}

void InstrumentSettings::createUI()
{
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --------------------------------------------------
    // Oscillator
    // --------------------------------------------------

    QGroupBox *oscillatorGroup =
        new QGroupBox("Oscillator", this);

    QFormLayout *oscillatorLayout =
        new QFormLayout(oscillatorGroup);

    m_oscillatorType =
        new QComboBox(this);

    m_oscillatorType->addItem("Sine");
    m_oscillatorType->addItem("Square");
    m_oscillatorType->addItem("Triangle");
    m_oscillatorType->addItem("Saw");

    m_frequency =
        new QDoubleSpinBox(this);

    m_frequency->setRange(20.0, 20000.0);
    m_frequency->setValue(440.0);
    m_frequency->setSuffix(" Hz");

    m_volume =
        new QDoubleSpinBox(this);

    m_volume->setRange(0.0, 1.0);
    m_volume->setSingleStep(0.01);
    m_volume->setValue(0.8);

    m_velocity =
        new QDoubleSpinBox(this);

    m_velocity->setRange(0.0, 1.0);
    m_velocity->setSingleStep(0.01);
    m_velocity->setValue(1.0);

    oscillatorLayout->addRow(
        "Waveform:",
        m_oscillatorType
    );

    oscillatorLayout->addRow(
        "Frequency:",
        m_frequency
    );

    oscillatorLayout->addRow(
        "Volume:",
        m_volume
    );

    oscillatorLayout->addRow(
        "Velocity:",
        m_velocity
    );

    // --------------------------------------------------
    // Audio
    // --------------------------------------------------

    QGroupBox *audioGroup =
        new QGroupBox("Audio", this);

    QFormLayout *audioLayout =
        new QFormLayout(audioGroup);

    m_sampleRate =
        new QSpinBox(this);

    m_sampleRate->setRange(8000, 192000);
    m_sampleRate->setSingleStep(1000);
    m_sampleRate->setValue(44100);

    m_sampleRate->setSuffix(" Hz");

    audioLayout->addRow(
        "Sample Rate:",
        m_sampleRate
    );

    // --------------------------------------------------
    // Effects
    // --------------------------------------------------

    QGroupBox *effectsGroup =
        new QGroupBox("Audio Effects", this);

    QVBoxLayout *effectsLayout =
        new QVBoxLayout(effectsGroup);

    m_reverb =
        new QCheckBox("Reverb", this);

    m_delay =
        new QCheckBox("Delay", this);

    m_distortion =
        new QCheckBox("Distortion", this);

    effectsLayout->addWidget(m_reverb);
    effectsLayout->addWidget(m_delay);
    effectsLayout->addWidget(m_distortion);

    // --------------------------------------------------
    // Delay
    // --------------------------------------------------

    QGroupBox *delayGroup =
        new QGroupBox("Delay Settings", this);

    QFormLayout *delayLayout =
        new QFormLayout(delayGroup);

    m_delayTime =
        new QDoubleSpinBox(this);

    m_delayTime->setRange(1.0, 5000.0);
    m_delayTime->setValue(300.0);
    m_delayTime->setSuffix(" ms");

    m_delayFeedback =
        new QDoubleSpinBox(this);

    m_delayFeedback->setRange(0.0, 0.99);
    m_delayFeedback->setSingleStep(0.01);
    m_delayFeedback->setValue(0.4);

    m_delayMix =
        new QDoubleSpinBox(this);

    m_delayMix->setRange(0.0, 1.0);
    m_delayMix->setSingleStep(0.01);
    m_delayMix->setValue(0.3);

    delayLayout->addRow(
        "Delay Time:",
        m_delayTime
    );

    delayLayout->addRow(
        "Feedback:",
        m_delayFeedback
    );

    delayLayout->addRow(
        "Mix:",
        m_delayMix
    );

    // --------------------------------------------------
    // Add everything
    // --------------------------------------------------

    mainLayout->addWidget(oscillatorGroup);
    mainLayout->addWidget(audioGroup);
    mainLayout->addWidget(effectsGroup);
    mainLayout->addWidget(delayGroup);

    mainLayout->addStretch();
    m_previewKeyboard = new PianoKeyboard(this);
    m_previewKeyboard->setFixedHeight(160);
    m_previewKeyboard->setStyleSheet(
    "background: red;"
);
    mainLayout->addWidget(m_previewKeyboard);
    // --------------------------------------------------
    // Signals
    // --------------------------------------------------

    connect(
    m_oscillatorType,
    &QComboBox::currentIndexChanged,
    this,
    [this](int index)
    {
        Oscillator::Waveform waveform;

        switch (index)
        {
            case 0:
                waveform = Oscillator::Waveform::Sine;
                break;

            case 1:
                waveform = Oscillator::Waveform::Saw;
                break;

            case 2:
                waveform = Oscillator::Waveform::Square;
                break;

            case 3:
                waveform = Oscillator::Waveform::Triangle;
                break;

            default:
                waveform = Oscillator::Waveform::Sine;
                break;
        }

        emit oscillatorTypeChanged(waveform);
    }
);

    connect(
        m_frequency,
        &QDoubleSpinBox::valueChanged,
        this,
        &InstrumentSettings::frequencyChanged
    );

    connect(
        m_volume,
        &QDoubleSpinBox::valueChanged,
        this,
        &InstrumentSettings::volumeChanged
    );

    connect(
        m_velocity,
        &QDoubleSpinBox::valueChanged,
        this,
        &InstrumentSettings::velocityChanged
    );

    connect(
        m_sampleRate,
        &QSpinBox::valueChanged,
        this,
        &InstrumentSettings::sampleRateChanged
    );

    connect(
        m_reverb,
        &QCheckBox::toggled,
        this,
        &InstrumentSettings::reverbChanged
    );

    connect(
        m_delay,
        &QCheckBox::toggled,
        this,
        &InstrumentSettings::delayChanged
    );

    connect(
        m_distortion,
        &QCheckBox::toggled,
        this,
        &InstrumentSettings::distortionChanged
    );

    connect(
        m_delayTime,
        &QDoubleSpinBox::valueChanged,
        this,
        &InstrumentSettings::delayTimeChanged
    );

    connect(
        m_delayFeedback,
        &QDoubleSpinBox::valueChanged,
        this,
        &InstrumentSettings::delayFeedbackChanged
    );

    connect(
        m_delayMix,
        &QDoubleSpinBox::valueChanged,
        this,
        &InstrumentSettings::delayMixChanged
    );

    connect(
    m_previewKeyboard,
    &PianoKeyboard::notePressed,
    this,
    &InstrumentSettings::previewNotePressed
    );

    connect(
    m_previewKeyboard,
    &PianoKeyboard::noteReleased,
    this,
    &InstrumentSettings::previewNoteReleased
    );
}