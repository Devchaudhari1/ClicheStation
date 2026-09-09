#pragma once

#include <QWidget>
#include "../synth/Oscillator.h"
#include "../audio/effects/AudioEffect.h"
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QSlider;
class QCheckBox;
class PianoKeyboard;
class QLabel;
class EffectChainEditor;

class InstrumentSettings : public QWidget
{
    Q_OBJECT

public:
    explicit InstrumentSettings(QWidget *parent = nullptr);

signals:
    void oscillatorTypeChanged(Oscillator::Waveform waveform);

    void frequencyChanged(float frequency);
    void volumeChanged(float volume);
    void velocityChanged(float velocity);

    void attackChanged(float seconds);
    void decayChanged(float seconds);
    void sustainChanged(float level);
    void releaseChanged(float seconds);
    void peakGainChanged(float level);

    void sampleRateChanged(int sampleRate);

    void reverbChanged(bool enabled);

    void reverbRoomSizeChanged(float roomSize);
    void reverbDampingChanged(float damping);
    void reverbWetChanged(float wet);
    void reverbDecayChanged(float decay);

    void delayChanged(bool enabled);
    void delayTimeChanged(double time);
    void delayFeedbackChanged(double feedback);
    void delayMixChanged(double mix);

    void distortionChanged(bool enabled);
    void distortionDriveChanged(float drive);
    void saturationChanged(bool enabled);

    void saturationDriveChanged(float drive);
    
    void flangerRateChanged(float rate);
    void flangerDepthChanged(float depth);
    void flangerFeedbackChanged(float feedback);
    void flangerMixChanged(float mix);

    void clippingThresholdChanged(float threshold);

    void compressionThresholdChanged(float threshold);
    void compressionRatioChanged(float ratio);
    void compressionAttackChanged(float attack);
    void compressionReleaseChanged(float release);
    void compressionMakeupGainChanged(float gain);

    void eqLowFrequencyChanged(float frequency);
    void eqLowGainChanged(float gain);

    void eqMidFrequencyChanged(float frequency);
    void eqMidGainChanged(float gain);
    void eqMidQChanged(float q);

    void eqHighFrequencyChanged(float frequency);
    void eqHighGainChanged(float gain);
    
    void previewNotePressed(int midiNote);
    void previewNoteReleased(int midiNote);

    void effectEnabledChanged(AudioEffectType effectType, bool enabled);
    void effectOrderChanged(const QVector<AudioEffectType> &order);
    
    void limiterThresholdChanged(float threshold);
    void limiterReleaseChanged(float release);
    void limiterCeilingChanged(float ceiling);

    void gateThresholdChanged(float threshold);
    void gateAttackChanged(float attack);
    void gateReleaseChanged(float release);
    void gateRangeChanged(float range);

    void chorusRateChanged(float rate);
    void chorusDepthChanged(float depth);
    void chorusMixChanged(float mix);

    void phaserRateChanged(float rate);
    void phaserDepthChanged(float depth);
    void phaserFeedbackChanged(float feedback);
    void phaserMixChanged(float mix);
private:
    void createUI();
    //Oscillator params
    QComboBox *m_oscillatorType;

    QDoubleSpinBox *m_frequency;
    QDoubleSpinBox *m_volume;
    QDoubleSpinBox *m_velocity;

    QSpinBox *m_sampleRate;
    // ADSR
    QDoubleSpinBox *m_attack;
    QDoubleSpinBox *m_decay;
    QDoubleSpinBox *m_sustain;
    QDoubleSpinBox *m_release;
    QDoubleSpinBox *m_peakGain;
    
    //Reverb params
    QDoubleSpinBox *m_reverbRoomSize;
    QDoubleSpinBox *m_reverbDamping;
    QDoubleSpinBox *m_reverbWet;
    QDoubleSpinBox *m_reverbDecay;
    //delay params
    QDoubleSpinBox *m_delayTime;
    QDoubleSpinBox *m_delayFeedback;
    QDoubleSpinBox *m_delayMix;
    //distortion params
    QSlider *m_distortionDrive;
    QLabel *m_distortionDriveLabel;
    //EQ params
    QDoubleSpinBox *m_eqLowFrequency;
    QDoubleSpinBox *m_eqLowGain;

    QDoubleSpinBox *m_eqMidFrequency;
    QDoubleSpinBox *m_eqMidGain;
    QDoubleSpinBox *m_eqMidQ;

    QDoubleSpinBox *m_eqHighFrequency;
    QDoubleSpinBox *m_eqHighGain;
    //saturation params
    QSlider *m_saturationDrive;
    QLabel *m_saturationDriveLabel;
    //flanger params
    QDoubleSpinBox *m_flangerRate;
    QDoubleSpinBox *m_flangerDepth;
    QDoubleSpinBox *m_flangerFeedback;
    QDoubleSpinBox *m_flangerMix;
    //compression params
    QDoubleSpinBox *m_compressionThreshold;
    QDoubleSpinBox *m_compressionRatio;
    QDoubleSpinBox *m_compressionAttack;
    QDoubleSpinBox *m_compressionRelease;
    QDoubleSpinBox *m_compressionMakeupGain;
    //clipping params
    QDoubleSpinBox *m_clippingThreshold;
    // Limiter
    QDoubleSpinBox *m_limiterThreshold;
    QDoubleSpinBox *m_limiterRelease;
    QDoubleSpinBox *m_limiterCeiling;

    // Gate
    QDoubleSpinBox *m_gateThreshold;
    QDoubleSpinBox *m_gateAttack;
    QDoubleSpinBox *m_gateRelease;
    QDoubleSpinBox *m_gateRange;

    // Chorus
    QDoubleSpinBox *m_chorusRate;
    QDoubleSpinBox *m_chorusDepth;
    QDoubleSpinBox *m_chorusMix;

    // Phaser
    QDoubleSpinBox *m_phaserRate;
    QDoubleSpinBox *m_phaserDepth;
    QDoubleSpinBox *m_phaserFeedback;
    QDoubleSpinBox *m_phaserMix;
    
    EffectChainEditor *m_effectChainEditor;
    PianoKeyboard *m_previewKeyboard;
};