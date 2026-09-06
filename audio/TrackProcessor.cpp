#include "TrackProcessor.h"

TrackProcessor::TrackProcessor(int trackId)
    : m_trackId(trackId)
{
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