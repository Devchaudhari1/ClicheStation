// AudioEffect.h

#pragma once

#include <cstddef>

class AudioEffect
{
public:
    virtual ~AudioEffect() = default;

    virtual void prepare(
        double sampleRate,
        std::size_t maximumBlockSize,
        std::size_t channels) = 0;

    virtual void process(
        float* left,
        float* right,
        std::size_t numSamples) = 0;

    virtual void reset() = 0;

    void setBypassed(bool bypassed)
    {
        m_bypassed = bypassed;
    }

    bool isBypassed() const
    {
        return m_bypassed;
    }

protected:
    bool m_bypassed = false;
};