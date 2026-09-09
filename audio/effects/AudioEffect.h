#pragma once

#include <cstddef>
#include <QString>
#include <atomic>

enum class AudioEffectType
{
    Clipping,
    Delay,
    Distortion,
    Saturation,
    Reverb,
    Flanger,
    Compression,
    EQ,
    Limiter,
    Gate,
    Chorus,
    Phaser
};

class AudioEffect
{
public:
    virtual ~AudioEffect() = default;

    virtual void prepare(double sampleRate,
                         std::size_t maximumBlockSize,
                         std::size_t channels) = 0;

    virtual void process(float *left,
                         float *right,
                         std::size_t numSamples) = 0;

    virtual void reset() = 0;

    virtual AudioEffectType effectType() const = 0;

    // Keep this for now.
    // Later this can be used purely for display/debugging,
    // or removed once the enum is fully established.
    virtual QString effectName() const = 0;

    void setBypassed(bool bypassed)
    {
        m_bypassed.store(bypassed);
    }

    bool isBypassed() const
    {
        return m_bypassed.load();
    }

protected:
    std::atomic<bool> m_bypassed{false};
};