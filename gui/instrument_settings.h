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

    void attackChanged(float attack);
    void decayChanged(float decay);
    void sustainChanged(float sustain);
    void releaseChanged(float release);

    void sampleRateChanged(int sampleRate);

    void reverbChanged(bool enabled);

    void delayChanged(bool enabled);
    void delayTimeChanged(double time);
    void delayFeedbackChanged(double feedback);
    void delayMixChanged(double mix);

    void distortionChanged(bool enabled);
    void distortionDriveChanged(float drive);
    void saturationChanged(bool enabled);

    void previewNotePressed(int midiNote);
    void previewNoteReleased(int midiNote);

    void effectEnabledChanged(AudioEffectType effectType, bool enabled);
    void effectOrderChanged(const QVector<AudioEffectType> &order);
private:
    void createUI();

    QComboBox *m_oscillatorType;

    QDoubleSpinBox *m_frequency;
    QDoubleSpinBox *m_volume;
    QDoubleSpinBox *m_velocity;

    QSpinBox *m_sampleRate;

    QCheckBox *m_reverb;
    QCheckBox *m_delay;
    QCheckBox *m_distortion;
    QCheckBox *m_saturation;

    QDoubleSpinBox *m_delayTime;
    QDoubleSpinBox *m_delayFeedback;
    QDoubleSpinBox *m_delayMix;

    QSlider *m_distortionDrive;
    QLabel *m_distortionDriveLabel;
    EffectChainEditor *m_effectChainEditor;
    PianoKeyboard *m_previewKeyboard;
};