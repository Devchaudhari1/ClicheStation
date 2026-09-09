#include "instrument_settings.h"
#include "effect_chain_editor.h"
#include "pianoKeyboard.h"
#include "../audio/effects/AudioEffect.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

InstrumentSettings::InstrumentSettings(QWidget *parent)
    : QWidget(parent, Qt::Window),
    m_effectChainEditor(nullptr)
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
    // ADSR
    QGroupBox *adsrGroup = new QGroupBox("ADSR", this);

    QFormLayout *adsrLayout = new QFormLayout(adsrGroup);

    m_attack =
        new QDoubleSpinBox(this);

    m_decay =
        new QDoubleSpinBox(this);

    m_sustain =
        new QDoubleSpinBox(this);

    m_release =
        new QDoubleSpinBox(this);

    m_peakGain =
        new QDoubleSpinBox(this);

    m_attack->setRange(0.0, 5000.0);
    m_attack->setValue(10.0);
    m_attack->setSuffix(" ms");
    m_attack->setDecimals(1);

    m_decay->setRange(0.0, 5000.0);
    m_decay->setValue(100.0);
    m_decay->setSuffix(" ms");
    m_decay->setDecimals(1);

    m_sustain->setRange(0.0, 1.0);
    m_sustain->setValue(0.8);
    m_sustain->setSingleStep(0.01);
    m_sustain->setDecimals(2);

    m_release->setRange(0.0, 5000.0);
    m_release->setValue(200.0);
    m_release->setSuffix(" ms");
    m_release->setDecimals(1);

    m_peakGain->setRange(0.0, 1.0);
    m_peakGain->setValue(1.0);
    m_peakGain->setSingleStep(0.01);
    m_peakGain->setDecimals(2);

    adsrLayout->addRow("Attack", m_attack);
    adsrLayout->addRow("Decay", m_decay);
    adsrLayout->addRow("Sustain", m_sustain);
    adsrLayout->addRow("Release", m_release);
    adsrLayout->addRow("Peak Gain", m_peakGain);
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

    QHBoxLayout *effectsLayout =
        new QHBoxLayout(effectsGroup);
    
    QVBoxLayout *chainLayout = new QVBoxLayout;
    QVBoxLayout *parameterLayout = new QVBoxLayout;

    m_effectChainEditor = new EffectChainEditor(this);

    m_effectChainEditor->addEffect(AudioEffectType::Clipping);
    m_effectChainEditor->addEffect(AudioEffectType::Delay);
    m_effectChainEditor->addEffect(AudioEffectType::Distortion);
    m_effectChainEditor->addEffect(AudioEffectType::Saturation);
    m_effectChainEditor->addEffect(AudioEffectType::Reverb);
    m_effectChainEditor->addEffect(AudioEffectType::Flanger);
    m_effectChainEditor->addEffect(AudioEffectType::Compression);
    m_effectChainEditor->addEffect(AudioEffectType::Limiter);
    m_effectChainEditor->addEffect(AudioEffectType::Gate);
    m_effectChainEditor->addEffect(AudioEffectType::Chorus);
    m_effectChainEditor->addEffect(AudioEffectType::Phaser);
    m_effectChainEditor->addEffect(AudioEffectType::EQ);

    chainLayout->addWidget(m_effectChainEditor);
    effectsLayout->addLayout(chainLayout, 1);

    //Clipping
    QGroupBox *clippingGroup = new QGroupBox("Clipping", this);
    m_clippingThreshold = new QDoubleSpinBox(this);
    m_clippingThreshold->setRange(0.01, 1.0);
    m_clippingThreshold->setSingleStep(0.01);
    m_clippingThreshold->setValue(1.0);
    m_clippingThreshold->setSuffix("");

    QFormLayout *clippingLayout = new QFormLayout(clippingGroup);
    clippingLayout->addRow("Clipping Threshold", m_clippingThreshold);

    //Distortion

    m_distortionDrive = new QSlider(Qt::Horizontal, this);
    m_distortionDrive->setRange(1, 20);
    m_distortionDrive->setValue(5);

    m_distortionDriveLabel = new QLabel("Distortion Drive: 5", this);
    QVBoxLayout * distortionLayout = new QVBoxLayout;
    distortionLayout->addWidget(m_distortionDriveLabel);
    distortionLayout->addWidget(m_distortionDrive);


    //Saturation

    m_saturationDrive =
        new QSlider(Qt::Horizontal, this);

    m_saturationDrive->setRange(1, 20);
    m_saturationDrive->setValue(5);

    m_saturationDriveLabel =new QLabel("Saturation: 5", this);

    QVBoxLayout * saturationLayout = new QVBoxLayout;
    saturationLayout->addWidget(m_saturationDriveLabel);

    saturationLayout->addWidget(m_saturationDrive);
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
    // Reverb
    // --------------------------------------------------

    QGroupBox *reverbGroup =
        new QGroupBox("Reverb Settings", this);

    QFormLayout *reverbLayout =
        new QFormLayout(reverbGroup);

    m_reverbRoomSize =
        new QDoubleSpinBox(this);

    m_reverbRoomSize->setRange(0.0, 1.0);
    m_reverbRoomSize->setSingleStep(0.01);
    m_reverbRoomSize->setValue(0.7);

    m_reverbDamping =
        new QDoubleSpinBox(this);

    m_reverbDamping->setRange(0.0, 1.0);
    m_reverbDamping->setSingleStep(0.01);
    m_reverbDamping->setValue(0.4);

    m_reverbWet =
        new QDoubleSpinBox(this);

    m_reverbWet->setRange(0.0, 1.0);
    m_reverbWet->setSingleStep(0.01);
    m_reverbWet->setValue(0.25);

    m_reverbDecay =
        new QDoubleSpinBox(this);

    m_reverbDecay->setRange(0.0, 1.0);
    m_reverbDecay->setSingleStep(0.01);
    m_reverbDecay->setValue(0.6);

    reverbLayout->addRow(
        "Room Size:",
        m_reverbRoomSize
    );

    reverbLayout->addRow(
        "Damping:",
        m_reverbDamping
    );

    reverbLayout->addRow(
        "Wet:",
        m_reverbWet
    );

    reverbLayout->addRow(
        "Decay:",
        m_reverbDecay
    );

    // --------------------------------------------------
    // Limiter
    // --------------------------------------------------

    QGroupBox *limiterGroup =
        new QGroupBox("Limiter", this);

    QFormLayout *limiterLayout =
        new QFormLayout(limiterGroup);

    m_limiterThreshold =
        new QDoubleSpinBox(this);

    m_limiterThreshold->setRange(-60.0, 0.0);
    m_limiterThreshold->setValue(-6.0);
    m_limiterThreshold->setSingleStep(1.0);
    m_limiterThreshold->setSuffix(" dB");

    m_limiterRelease =
        new QDoubleSpinBox(this);

    m_limiterRelease->setRange(1.0, 1000.0);
    m_limiterRelease->setValue(100.0);
    m_limiterRelease->setSingleStep(1.0);
    m_limiterRelease->setSuffix(" ms");

    m_limiterCeiling =
        new QDoubleSpinBox(this);

    m_limiterCeiling->setRange(-20.0, 0.0);
    m_limiterCeiling->setValue(-0.1);
    m_limiterCeiling->setSingleStep(0.1);
    m_limiterCeiling->setSuffix(" dB");

    limiterLayout->addRow(
        "Threshold",
        m_limiterThreshold);

    limiterLayout->addRow(
        "Release",
        m_limiterRelease);

    limiterLayout->addRow(
        "Ceiling",
        m_limiterCeiling);


    // --------------------------------------------------
    // Gate
    // --------------------------------------------------

    QGroupBox *gateGroup =
        new QGroupBox("Gate", this);

    QFormLayout *gateLayout =
        new QFormLayout(gateGroup);

    m_gateThreshold =
        new QDoubleSpinBox(this);

    m_gateThreshold->setRange(-80.0, 0.0);
    m_gateThreshold->setValue(-40.0);
    m_gateThreshold->setSingleStep(1.0);
    m_gateThreshold->setSuffix(" dB");

    m_gateAttack =
        new QDoubleSpinBox(this);

    m_gateAttack->setRange(0.1, 1000.0);
    m_gateAttack->setValue(5.0);
    m_gateAttack->setSingleStep(0.1);
    m_gateAttack->setSuffix(" ms");

    m_gateRelease =
        new QDoubleSpinBox(this);

    m_gateRelease->setRange(1.0, 2000.0);
    m_gateRelease->setValue(100.0);
    m_gateRelease->setSingleStep(1.0);
    m_gateRelease->setSuffix(" ms");

    m_gateRange =
        new QDoubleSpinBox(this);

    m_gateRange->setRange(-80.0, 0.0);
    m_gateRange->setValue(-60.0);
    m_gateRange->setSingleStep(1.0);
    m_gateRange->setSuffix(" dB");

    gateLayout->addRow(
        "Threshold",
        m_gateThreshold);

    gateLayout->addRow(
        "Attack",
        m_gateAttack);

    gateLayout->addRow(
        "Release",
        m_gateRelease);

    gateLayout->addRow(
        "Range",
        m_gateRange);


    // --------------------------------------------------
    // Chorus
    // --------------------------------------------------

    QGroupBox *chorusGroup =
        new QGroupBox("Chorus", this);

    QFormLayout *chorusLayout =
        new QFormLayout(chorusGroup);

    m_chorusRate =
        new QDoubleSpinBox(this);

    m_chorusRate->setRange(0.01, 10.0);
    m_chorusRate->setValue(0.8);
    m_chorusRate->setSingleStep(0.01);
    m_chorusRate->setSuffix(" Hz");

    m_chorusDepth =
        new QDoubleSpinBox(this);

    m_chorusDepth->setRange(0.0, 20.0);
    m_chorusDepth->setValue(15.0);
    m_chorusDepth->setSingleStep(0.1);
    m_chorusDepth->setSuffix(" ms");

    m_chorusMix =
        new QDoubleSpinBox(this);

    m_chorusMix->setRange(0.0, 1.0);
    m_chorusMix->setValue(0.5);
    m_chorusMix->setSingleStep(0.05);

    chorusLayout->addRow(
        "Rate",
        m_chorusRate);

    chorusLayout->addRow(
        "Depth",
        m_chorusDepth);

    chorusLayout->addRow(
        "Mix",
        m_chorusMix);


    // --------------------------------------------------
    // Phaser
    // --------------------------------------------------

    QGroupBox *phaserGroup =
        new QGroupBox("Phaser", this);

    QFormLayout *phaserLayout =
        new QFormLayout(phaserGroup);

    m_phaserRate =
        new QDoubleSpinBox(this);

    m_phaserRate->setRange(0.01, 10.0);
    m_phaserRate->setValue(0.5);
    m_phaserRate->setSingleStep(0.01);
    m_phaserRate->setSuffix(" Hz");

    m_phaserDepth =
        new QDoubleSpinBox(this);

    m_phaserDepth->setRange(0.0, 1.0);
    m_phaserDepth->setValue(0.7);
    m_phaserDepth->setSingleStep(0.05);

    m_phaserFeedback =
        new QDoubleSpinBox(this);

    m_phaserFeedback->setRange(-0.95, 0.95);
    m_phaserFeedback->setValue(0.2);
    m_phaserFeedback->setSingleStep(0.05);

    m_phaserMix =
        new QDoubleSpinBox(this);

    m_phaserMix->setRange(0.0, 1.0);
    m_phaserMix->setValue(0.5);
    m_phaserMix->setSingleStep(0.05);

    phaserLayout->addRow(
        "Rate",
        m_phaserRate);

    phaserLayout->addRow(
        "Depth",
        m_phaserDepth);

    phaserLayout->addRow(
        "Feedback",
        m_phaserFeedback);

    phaserLayout->addRow(
        "Mix",
        m_phaserMix);

    //Flanger

    QGroupBox *flangerGroup =
        new QGroupBox("Flanger", this);

    QFormLayout *flangerLayout =
        new QFormLayout(flangerGroup);

    m_flangerRate =
        new QDoubleSpinBox(flangerGroup);

    m_flangerRate->setRange(0.01, 10.0);
    m_flangerRate->setValue(0.25);
    m_flangerRate->setSingleStep(0.01);
    m_flangerRate->setSuffix(" Hz");

    m_flangerDepth =
        new QDoubleSpinBox(flangerGroup);

    m_flangerDepth->setRange(0.0, 0.004);
    m_flangerDepth->setValue(0.002);
    m_flangerDepth->setSingleStep(0.0001);
    m_flangerDepth->setDecimals(4);
    m_flangerDepth->setSuffix(" s");

    m_flangerFeedback =
        new QDoubleSpinBox(flangerGroup);

    m_flangerFeedback->setRange(-0.95, 0.95);
    m_flangerFeedback->setValue(0.2);
    m_flangerFeedback->setSingleStep(0.05);

    m_flangerMix =
        new QDoubleSpinBox(flangerGroup);

    m_flangerMix->setRange(0.0, 1.0);
    m_flangerMix->setValue(0.5);
    m_flangerMix->setSingleStep(0.05);

    flangerLayout->addRow(
        "Rate",
        m_flangerRate);

    flangerLayout->addRow(
        "Depth",
        m_flangerDepth);

    flangerLayout->addRow(
        "Feedback",
        m_flangerFeedback);

    flangerLayout->addRow(
        "Mix",
        m_flangerMix);

    // Compression
    QGroupBox *compressionGroup =
        new QGroupBox("Compression", this);

    QFormLayout *compressionLayout =
        new QFormLayout(compressionGroup);

    m_compressionThreshold =
        new QDoubleSpinBox(compressionGroup);

    m_compressionThreshold->setRange(-60.0, 0.0);
    m_compressionThreshold->setValue(-18.0);
    m_compressionThreshold->setSingleStep(1.0);
    m_compressionThreshold->setSuffix(" dB");

    m_compressionRatio =
        new QDoubleSpinBox(compressionGroup);

    m_compressionRatio->setRange(1.0, 20.0);
    m_compressionRatio->setValue(4.0);
    m_compressionRatio->setSingleStep(0.5);
    m_compressionRatio->setSuffix(":1");

    m_compressionAttack =
        new QDoubleSpinBox(compressionGroup);

    m_compressionAttack->setRange(0.0001, 1.0);
    m_compressionAttack->setValue(0.01);
    m_compressionAttack->setSingleStep(0.001);
    m_compressionAttack->setDecimals(4);
    m_compressionAttack->setSuffix(" s");

    m_compressionRelease =
        new QDoubleSpinBox(compressionGroup);

    m_compressionRelease->setRange(0.001, 1.0);
    m_compressionRelease->setValue(0.1);
    m_compressionRelease->setSingleStep(0.01);
    m_compressionRelease->setDecimals(3);
    m_compressionRelease->setSuffix(" s");

    m_compressionMakeupGain =
        new QDoubleSpinBox(compressionGroup);

    m_compressionMakeupGain->setRange(0.0, 10.0);
    m_compressionMakeupGain->setValue(1.0);
    m_compressionMakeupGain->setSingleStep(0.1);

    compressionLayout->addRow(
        "Threshold",
        m_compressionThreshold);

    compressionLayout->addRow(
        "Ratio",
        m_compressionRatio);

    compressionLayout->addRow(
        "Attack",
        m_compressionAttack);

    compressionLayout->addRow(
        "Release",
        m_compressionRelease);

    compressionLayout->addRow(
        "Makeup Gain",
        m_compressionMakeupGain);

    //EQ

    QGroupBox *eqGroup = new QGroupBox("EQ", this);
    QFormLayout *eqLayout = new QFormLayout(eqGroup);

    m_eqLowFrequency = new QDoubleSpinBox(this);
    m_eqLowFrequency->setRange(20.0, 20000.0);
    m_eqLowFrequency->setValue(100.0);
    m_eqLowFrequency->setSuffix(" Hz");

    m_eqLowGain = new QDoubleSpinBox(this);
    m_eqLowGain->setRange(-24.0, 24.0);
    m_eqLowGain->setValue(0.0);
    m_eqLowGain->setSuffix(" dB");

    m_eqMidFrequency = new QDoubleSpinBox(this);
    m_eqMidFrequency->setRange(20.0, 20000.0);
    m_eqMidFrequency->setValue(1000.0);
    m_eqMidFrequency->setSuffix(" Hz");

    m_eqMidGain = new QDoubleSpinBox(this);
    m_eqMidGain->setRange(-24.0, 24.0);
    m_eqMidGain->setValue(0.0);
    m_eqMidGain->setSuffix(" dB");

    m_eqMidQ = new QDoubleSpinBox(this);
    m_eqMidQ->setRange(0.1, 10.0);
    m_eqMidQ->setValue(1.0);

    m_eqHighFrequency = new QDoubleSpinBox(this);
    m_eqHighFrequency->setRange(20.0, 20000.0);
    m_eqHighFrequency->setValue(8000.0);
    m_eqHighFrequency->setSuffix(" Hz");

    m_eqHighGain = new QDoubleSpinBox(this);
    m_eqHighGain->setRange(-24.0, 24.0);
    m_eqHighGain->setValue(0.0);
    m_eqHighGain->setSuffix(" dB");

    eqLayout->addRow("Low Frequency", m_eqLowFrequency);
    eqLayout->addRow("Low Gain", m_eqLowGain);

    eqLayout->addRow("Mid Frequency", m_eqMidFrequency);
    eqLayout->addRow("Mid Gain", m_eqMidGain);
    eqLayout->addRow("Mid Q", m_eqMidQ);

    eqLayout->addRow("High Frequency", m_eqHighFrequency);
    eqLayout->addRow("High Gain", m_eqHighGain);
    
    // --------------------------------------------------
    // Add everything
    // --------------------------------------------------

    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *midLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QHBoxLayout *driveLayout = new QHBoxLayout;
    QHBoxLayout *dynamicsLayout = new QHBoxLayout;

    // Row 1
    topLayout->addWidget(oscillatorGroup);
    topLayout->addWidget(delayGroup);
    topLayout->addWidget(reverbGroup);
    topLayout->addStretch();

    // Row 2
    midLayout->addWidget(eqGroup);
    midLayout->addWidget(adsrGroup);
    midLayout->addWidget(clippingGroup);
    midLayout->addStretch();

    // Row 3
    bottomLayout->addWidget(flangerGroup);
    bottomLayout->addWidget(compressionGroup);
    bottomLayout->addStretch();

    // Row 4
    dynamicsLayout->addWidget(limiterGroup);
    dynamicsLayout->addWidget(gateGroup);
    dynamicsLayout->addWidget(chorusGroup);
    dynamicsLayout->addWidget(phaserGroup);
    dynamicsLayout->addStretch();

    //Row 5
    driveLayout->addLayout(distortionLayout);
    driveLayout->addLayout(saturationLayout);

    // Put parameter rows on the right
    parameterLayout->addLayout(topLayout);
    parameterLayout->addLayout(midLayout);
    parameterLayout->addLayout(bottomLayout);
    parameterLayout->addLayout(dynamicsLayout);
    parameterLayout->addLayout(driveLayout);

    parameterLayout->addStretch();

    // Add the right-hand parameter area
    effectsLayout->addLayout(parameterLayout, 3);

    // Audio belongs to the main window, not the effects layout
    mainLayout->addWidget(audioGroup);

    // Audio Effects
    mainLayout->addWidget(effectsGroup);

    // Preview keyboard
    m_previewKeyboard = new PianoKeyboard(this);
    m_previewKeyboard->setFixedHeight(160);
    m_previewKeyboard->setStyleSheet(
        "background: red;"
    );

    mainLayout->addWidget(m_previewKeyboard);
    // --------------------------------------------------
    // Signals
    // --------------------------------------------------

    //Oscillator 
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
            waveform = Oscillator::Waveform::Square;
            break;

        case 2:
            waveform = Oscillator::Waveform::Triangle;
            break;

        case 3:
            waveform = Oscillator::Waveform::Saw;
            break;

        default:
            waveform = Oscillator::Waveform::Sine;
            break;
    }

        emit oscillatorTypeChanged(waveform);
    }
);
    // Oscillator
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

    //ADSR

    connect(
        m_attack,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit attackChanged(
                static_cast<float>(value / 1000.0)
            );
        }
    );

    connect(
        m_decay,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit decayChanged(
                static_cast<float>(value / 1000.0)
            );
        }
    );

    connect(
        m_sustain,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit sustainChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_release,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit releaseChanged(
                static_cast<float>(value / 1000.0)
            );
        }
    );

    connect(
        m_peakGain,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit peakGainChanged(
                static_cast<float>(value)
            );
        }
    );
    //Effect Chain Editor Signals
    connect(
        m_effectChainEditor,
        &EffectChainEditor::effectEnabledChanged,
        this,
        &InstrumentSettings::effectEnabledChanged
    );

    connect(
        m_effectChainEditor,
        &EffectChainEditor::effectOrderChanged,
        this,
        &InstrumentSettings::effectOrderChanged
    );
    //  Distortion Signals
    connect(
        m_distortionDrive,
        &QSlider::valueChanged,
        this,
        [this](int value)
        {
            m_distortionDriveLabel->setText(
                QString("Drive: %1").arg(value)
            );

            emit distortionDriveChanged(static_cast<float>(value));
        }
    );
    //  Saturation Signals
    connect(
        m_saturationDrive,
        &QSlider::valueChanged,
        this,
        [this](int value)
        {
            m_saturationDriveLabel->setText(
                QString("Drive: %1").arg(value)
            );

            emit saturationDriveChanged(
                static_cast<float>(value)
            );
        }
    );
    // Delay signals
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
    //Reverb signals
    connect(
        m_reverbRoomSize,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit reverbRoomSizeChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_reverbDamping,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit reverbDampingChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_reverbWet,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit reverbWetChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_reverbDecay,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit reverbDecayChanged(
                static_cast<float>(value)
            );
        }
    );
    // Flanger

    connect(
        m_flangerRate,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit flangerRateChanged(
                static_cast<float>(value));
        });

    connect(
        m_flangerDepth,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit flangerDepthChanged(
                static_cast<float>(value));
        });

    connect(
        m_flangerFeedback,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit flangerFeedbackChanged(
                static_cast<float>(value));
        });

    connect(
        m_flangerMix,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit flangerMixChanged(
                static_cast<float>(value));
        });

    // Compression
    connect(
        m_compressionThreshold,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit compressionThresholdChanged(
                static_cast<float>(value));
        });

    connect(
        m_compressionRatio,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit compressionRatioChanged(
                static_cast<float>(value));
        });

    connect(
        m_compressionAttack,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit compressionAttackChanged(
                static_cast<float>(value));
        });

    connect(
        m_compressionRelease,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit compressionReleaseChanged(
                static_cast<float>(value));
        });

    connect(
        m_compressionMakeupGain,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            emit compressionMakeupGainChanged(
                static_cast<float>(value));
        });
    //EQ
    connect(
        m_eqLowFrequency,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit eqLowFrequencyChanged(static_cast<float>(value));
        }
    );

    connect(
        m_eqLowGain,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit eqLowGainChanged(static_cast<float>(value));
        }
    );

    connect(
        m_eqMidFrequency,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit eqMidFrequencyChanged(static_cast<float>(value));
        }
    );

    connect(
        m_eqMidGain,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit eqMidGainChanged(static_cast<float>(value));
        }
    );

    connect(
        m_eqMidQ,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit eqMidQChanged(static_cast<float>(value));
        }
    );

    connect(
        m_eqHighFrequency,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit eqHighFrequencyChanged(static_cast<float>(value));
        }
    );

    connect(
        m_eqHighGain,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit eqHighGainChanged(static_cast<float>(value));
        }
    );
    // --------------------------------------------------
    // Limiter
    // --------------------------------------------------

    connect(
        m_limiterThreshold,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit limiterThresholdChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_limiterRelease,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit limiterReleaseChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_limiterCeiling,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit limiterCeilingChanged(
                static_cast<float>(value)
            );
        }
    );
    // --------------------------------------------------
    // Gate
    // --------------------------------------------------

    connect(
        m_gateThreshold,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit gateThresholdChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_gateAttack,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit gateAttackChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_gateRelease,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit gateReleaseChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_gateRange,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit gateRangeChanged(
                static_cast<float>(value)
            );
        }
    );

    // --------------------------------------------------
    // Chorus
    // --------------------------------------------------

    connect(
        m_chorusRate,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit chorusRateChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_chorusDepth,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit chorusDepthChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_chorusMix,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit chorusMixChanged(
                static_cast<float>(value)
            );
        }
    );
    // --------------------------------------------------
    // Phaser
    // --------------------------------------------------

    connect(
        m_phaserRate,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit phaserRateChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_phaserDepth,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit phaserDepthChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_phaserFeedback,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit phaserFeedbackChanged(
                static_cast<float>(value)
            );
        }
    );

    connect(
        m_phaserMix,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit phaserMixChanged(
                static_cast<float>(value)
            );
        }
    );
    // Clipping
    connect(
        m_clippingThreshold,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        [this](double value)
        {
            emit clippingThresholdChanged(static_cast<float>(value));
        }
    );
    //preview Keyboard signals
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