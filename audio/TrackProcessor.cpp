#include "TrackProcessor.h"
#include <QDebug>

#include "TrackProcessor.h"

#include "effects/Clipping.h"
#include "effects/Delay.h"
#include "effects/Distortion.h"
#include "effects/Saturation.h"

#include <memory>

TrackProcessor::TrackProcessor(int trackId)
    : m_trackId(trackId)
{
    m_effectChain.addEffect(AudioEffectType::Clipping);
    m_effectChain.addEffect(AudioEffectType::Delay);
    m_effectChain.addEffect(AudioEffectType::Distortion);
    m_effectChain.addEffect(AudioEffectType::Saturation);
    m_effectChain.addEffect(AudioEffectType::Reverb);
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
        // qDebug() << "Before effects:"
        //      << left[0]
        //      << left[numSamples / 2];
    m_effectChain.process(left, right, numSamples);
        // qDebug() << "After effects:"
        //      << left[0]
        //      << left[numSamples / 2];
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