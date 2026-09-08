// Distortion.h

#pragma once

#include "AudioEffect.h"
#include <atomic>
#include <QString>

class Distortion : public AudioEffect
{
public:
    void prepare(
        double sampleRate,
        std::size_t maximumBlockSize,
        std::size_t channels) override;

    void process(
        float* left,
        float* right,
        std::size_t numSamples) override;

    void reset() override;
    
    void setDrive(float drive);

    AudioEffectType effectType() const override;
    
    QString effectName() const override;
private:
    std::atomic<float> m_drive { 1.0f };
};