#include "TrackProcessor.h"
#include <QDebug>

#include "TrackProcessor.h"

#include "effects/Clipping.h"
#include "effects/Delay.h"
#include "effects/Distortion.h"
#include "effects/EQ.h"
#include "effects/Reverb.h"
#include "effects/Saturation.h"
#include "effects/Flanger.h"
#include "effects/Compression.h"
#include "effects/Limiter.h"
#include "effects/Gate.h"
#include "effects/Phaser.h"
#include "effects/Chorus.h"


#include <memory>

TrackProcessor::TrackProcessor(int trackId)
    : m_trackId(trackId)
{
    m_effectChain.addEffect(AudioEffectType::Clipping);
    m_effectChain.addEffect(AudioEffectType::Delay);
    m_effectChain.addEffect(AudioEffectType::Distortion);
    m_effectChain.addEffect(AudioEffectType::Saturation);
    m_effectChain.addEffect(AudioEffectType::Reverb);
    m_effectChain.addEffect(AudioEffectType::Flanger);
    m_effectChain.addEffect(AudioEffectType::Compression);
    m_effectChain.addEffect(AudioEffectType::Limiter);
    m_effectChain.addEffect(AudioEffectType::Gate);
    m_effectChain.addEffect(AudioEffectType::Chorus);
    m_effectChain.addEffect(AudioEffectType::Phaser);
    m_effectChain.addEffect(AudioEffectType::EQ);
}

void TrackProcessor::setState(TrackState state)
{
    m_state = state;
}

TrackState TrackProcessor::state() const
{
    return m_state;
}

int TrackProcessor::trackId() const
{
    return m_trackId;
}

void TrackProcessor::prepare(double sampleRate,
                             std::size_t blockSize)
{
    m_synth.prepare(sampleRate, blockSize);

    m_effectChain.prepare(sampleRate, blockSize, 2);
    
}

void TrackProcessor::noteOn(int midiNote, int velocity)
{
    m_synth.noteOn(midiNote, velocity);
}

void TrackProcessor::noteOff(int midiNote)
{
    m_synth.noteOff(midiNote);
}

void TrackProcessor::pitchBend(int channel, int value)
{
    m_synth.pitchBend(channel, value);
}

void TrackProcessor::render(float *left,
                            float *right,
                            std::size_t numSamples)
{
    m_synth.render(left, right, numSamples);

    m_effectChain.process(left, right, numSamples);
}

QVector<float> TrackProcessor::renderClip(
    const QVector<PlacedNote>& notes)
{
    QVector<float> left;
    QVector<float> right;

    m_synth.render(notes, left, right);

    if (left.isEmpty())
        return {};

    m_effectChain.process(
        left.data(),
        right.data(),
        static_cast<std::size_t>(left.size())
    );

    return left;
}

void TrackProcessor::setEffectEnabled(
    AudioEffectType effectType,
    bool enabled)
{
    m_effectChain.setEffectEnabled(
        effectType,
        enabled
    );
}

void TrackProcessor::setEffectOrder(
    const QVector<AudioEffectType> &order)
{
    m_effectChain.setEffectOrder(order);
}

void TrackProcessor::setClippingThreshold(float threshold)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::Clipping);

    auto *clipping = dynamic_cast<Clipping *>(effect);

    if (!clipping)
        return;

    clipping->setThreshold(threshold);
}

void TrackProcessor::setDelayTime(double time)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Delay
        );

    auto *delay =
        dynamic_cast<Delay *>(effect);

    if (!delay)
        return;

    delay->setDelayTime(time);
}

void TrackProcessor::setDelayFeedback(double feedback)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Delay
        );

    auto *delay =
        dynamic_cast<Delay *>(effect);

    if (!delay)
        return;

    delay->setFeedback(feedback);
}

void TrackProcessor::setDelayMix(double mix)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Delay
        );

    auto *delay =
        dynamic_cast<Delay *>(effect);

    if (!delay)
        return;

    delay->setMix(mix);
}

void TrackProcessor::setReverbRoomSize(float roomSize)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Reverb
        );

    auto *reverb =
        dynamic_cast<Reverb *>(effect);

    if (!reverb)
        return;

    reverb->setRoomSize(roomSize);
}

void TrackProcessor::setReverbDamping(float damping)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Reverb
        );

    auto *reverb =
        dynamic_cast<Reverb *>(effect);

    if (!reverb)
        return;

    reverb->setDamping(damping);
}

void TrackProcessor::setReverbWet(float wet)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Reverb
        );

    auto *reverb =
        dynamic_cast<Reverb *>(effect);

    if (!reverb)
        return;

    reverb->setWet(wet);
}

void TrackProcessor::setReverbDecay(float decay)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Reverb
        );

    auto *reverb =
        dynamic_cast<Reverb *>(effect);

    if (!reverb)
        return;

    reverb->setDecay(decay);
}

void TrackProcessor::setSaturationDrive(float drive)
{
    auto *effect =
        m_effectChain.findEffect(
            AudioEffectType::Saturation
        );

    auto *saturation =
        dynamic_cast<Saturation *>(effect);

    if (!saturation)
        return;

    saturation->setDrive(drive);
}

void TrackProcessor::setFlangerRate(float rate)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Flanger);

    auto *flanger =
        dynamic_cast<Flanger *>(effect);

    if (!flanger)
        return;

    flanger->setRate(rate);
}

void TrackProcessor::setFlangerDepth(float depth)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Flanger);

    auto *flanger =
        dynamic_cast<Flanger *>(effect);

    if (!flanger)
        return;

    flanger->setDepth(depth);
}

void TrackProcessor::setFlangerFeedback(float feedback)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Flanger);

    auto *flanger =
        dynamic_cast<Flanger *>(effect);

    if (!flanger)
        return;

    flanger->setFeedback(feedback);
}

void TrackProcessor::setFlangerMix(float mix)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Flanger);

    auto *flanger =
        dynamic_cast<Flanger *>(effect);

    if (!flanger)
        return;

    flanger->setMix(mix);
}

void TrackProcessor::setCompressionThreshold(float threshold)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Compression);

    auto *compression =
        dynamic_cast<Compression *>(effect);

    if (!compression)
        return;

    compression->setThreshold(threshold);
}

void TrackProcessor::setCompressionRatio(float ratio)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Compression);

    auto *compression =
        dynamic_cast<Compression *>(effect);

    if (!compression)
        return;

    compression->setRatio(ratio);
}

void TrackProcessor::setCompressionAttack(float attack)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Compression);

    auto *compression =
        dynamic_cast<Compression *>(effect);

    if (!compression)
        return;

    compression->setAttack(attack);
}

void TrackProcessor::setCompressionRelease(float release)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Compression);

    auto *compression =
        dynamic_cast<Compression *>(effect);

    if (!compression)
        return;

    compression->setRelease(release);
}

void TrackProcessor::setCompressionMakeupGain(float gain)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Compression);

    auto *compression =
        dynamic_cast<Compression *>(effect);

    if (!compression)
        return;

    compression->setMakeupGain(gain);
}

void TrackProcessor::setDistortionDrive(float drive)
{
    auto *effect = m_effectChain.findEffect(
        AudioEffectType::Distortion
    );

    auto *distortion = dynamic_cast<Distortion *>(effect);

    if (!distortion)
        return;

    distortion->setDrive(drive);
}

void TrackProcessor::setEQLowFrequency(float frequency)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::EQ);
    auto *eq = dynamic_cast<EQ *>(effect);

    if (!eq)
        return;

    eq->setLowFrequency(frequency);
}

void TrackProcessor::setEQLowGain(float gain)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::EQ);
    auto *eq = dynamic_cast<EQ *>(effect);

    if (!eq)
        return;

    eq->setLowGain(gain);
}

void TrackProcessor::setEQMidFrequency(float frequency)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::EQ);
    auto *eq = dynamic_cast<EQ *>(effect);

    if (!eq)
        return;

    eq->setMidFrequency(frequency);
}

void TrackProcessor::setEQMidGain(float gain)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::EQ);
    auto *eq = dynamic_cast<EQ *>(effect);

    if (!eq)
        return;

    eq->setMidGain(gain);
}

void TrackProcessor::setEQMidQ(float q)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::EQ);
    auto *eq = dynamic_cast<EQ *>(effect);

    if (!eq)
        return;

    eq->setMidQ(q);
}

void TrackProcessor::setEQHighFrequency(float frequency)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::EQ);
    auto *eq = dynamic_cast<EQ *>(effect);

    if (!eq)
        return;

    eq->setHighFrequency(frequency);
}

void TrackProcessor::setEQHighGain(float gain)
{
    auto *effect = m_effectChain.findEffect(AudioEffectType::EQ);
    auto *eq = dynamic_cast<EQ *>(effect);

    if (!eq)
        return;

    eq->setHighGain(gain);
}

void TrackProcessor::setLimiterThreshold(float threshold)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Limiter);

    auto *limiter =
        dynamic_cast<Limiter *>(effect);

    if (!limiter)
        return;

    limiter->setThreshold(threshold);
}

void TrackProcessor::setLimiterRelease(float release)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Limiter);

    auto *limiter =
        dynamic_cast<Limiter *>(effect);

    if (!limiter)
        return;

    limiter->setRelease(release);
}

void TrackProcessor::setLimiterCeiling(float ceiling)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Limiter);

    auto *limiter =
        dynamic_cast<Limiter *>(effect);

    if (!limiter)
        return;

    limiter->setCeiling(ceiling);
}

void TrackProcessor::setGateThreshold(float threshold)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Gate);

    auto *gate =
        dynamic_cast<Gate *>(effect);

    if (!gate)
        return;

    gate->setThreshold(threshold);
}

void TrackProcessor::setGateAttack(float attack)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Gate);

    auto *gate =
        dynamic_cast<Gate *>(effect);

    if (!gate)
        return;

    gate->setAttack(attack);
}

void TrackProcessor::setGateRelease(float release)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Gate);

    auto *gate =
        dynamic_cast<Gate *>(effect);

    if (!gate)
        return;

    gate->setRelease(release);
}

void TrackProcessor::setGateRange(float range)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Gate);

    auto *gate =
        dynamic_cast<Gate *>(effect);

    if (!gate)
        return;

    gate->setRange(range);
}

void TrackProcessor::setChorusRate(float rate)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Chorus);

    auto *chorus =
        dynamic_cast<Chorus *>(effect);

    if (!chorus)
        return;

    chorus->setRate(rate);
}

void TrackProcessor::setChorusDepth(float depth)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Chorus);

    auto *chorus =
        dynamic_cast<Chorus *>(effect);

    if (!chorus)
        return;

    chorus->setDepth(depth);
}

void TrackProcessor::setChorusMix(float mix)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Chorus);

    auto *chorus =
        dynamic_cast<Chorus *>(effect);

    if (!chorus)
        return;

    chorus->setMix(mix);
}

void TrackProcessor::setPhaserRate(float rate)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Phaser);

    auto *phaser =
        dynamic_cast<Phaser *>(effect);

    if (!phaser)
        return;

    phaser->setRate(rate);
}

void TrackProcessor::setPhaserDepth(float depth)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Phaser);

    auto *phaser =
        dynamic_cast<Phaser *>(effect);

    if (!phaser)
        return;

    phaser->setDepth(depth);
}

void TrackProcessor::setPhaserFeedback(float feedback)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Phaser);

    auto *phaser =
        dynamic_cast<Phaser *>(effect);

    if (!phaser)
        return;

    phaser->setFeedback(feedback);
}

void TrackProcessor::setPhaserMix(float mix)
{
    auto *effect =
        m_effectChain.findEffect(AudioEffectType::Phaser);

    auto *phaser =
        dynamic_cast<Phaser *>(effect);

    if (!phaser)
        return;

    phaser->setMix(mix);
}

void TrackProcessor::setFrequency(float frequency)
{
    m_synth.setFrequency(frequency);
}

void TrackProcessor::setVolume(float volume)
{
    m_synth.setVolume(volume);
}

void TrackProcessor::setWaveform(
    Oscillator::Waveform waveform)
{
    m_synth.setWaveform(waveform);
}

void TrackProcessor::setVelocity(float velocity)
{
    m_synth.setVelocity(velocity);
}

void TrackProcessor::setAttack(float seconds)
{
    m_synth.setAttack(seconds);
}

void TrackProcessor::setDecay(float seconds)
{
    m_synth.setDecay(seconds);
}

void TrackProcessor::setSustain(float level)
{
    m_synth.setSustain(level);
}

void TrackProcessor::setRelease(float seconds)
{
    m_synth.setRelease(seconds);
}

void TrackProcessor::setPeakGain(float level)
{
    m_synth.setPeakGain(level);
}