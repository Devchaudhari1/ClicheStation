// EffectChain.h

#pragma once

#include "AudioEffect.h"
#include "AudioEffectFactory.h"
#include <memory>
#include <vector>
#include <QString>
#include <QVector>

class EffectChain
{
public:
    void addEffect(
        std::unique_ptr<AudioEffect> effect);

    void prepare(
        double sampleRate,
        std::size_t blockSize,
        std::size_t channels);

    void process(
        float* left,
        float* right,
        std::size_t numSamples);

    void reset();

    void addEffect(AudioEffectType type);

    AudioEffect *findEffect(AudioEffectType type);

    void setEffectEnabled(
        AudioEffectType effectType,
        bool enabled
    );

    void setEffectOrder(
        const QVector<AudioEffectType> &order
    );
private:
    std::vector<std::unique_ptr<AudioEffect>> effects;
};