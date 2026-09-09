// EffectChain.cpp

#include "EffectChain.h"
#include <QDebug>

void EffectChain::addEffect(
    std::unique_ptr<AudioEffect> effect)
{
    effects.push_back(std::move(effect));
}

void EffectChain::prepare(
    double sampleRate,
    std::size_t blockSize,
    std::size_t channels)
{
    for (auto& effect : effects)
    {
        effect->prepare(
            sampleRate,
            blockSize,
            channels);
    }
}

void EffectChain::process(
    float* left,
    float* right,
    std::size_t numSamples)
{
    for (auto& effect : effects)
    {
        // qDebug() << "Effect bypassed:"
        //          << effect->isBypassed();
        if (!effect->isBypassed())
        {
            effect->process(
                left,
                right,
                numSamples);
        }
    }
}

void EffectChain::reset()
{
    for (auto& effect : effects)
        effect->reset();
}

void EffectChain::setEffectEnabled(
    AudioEffectType effectType,
    bool enabled)
{
        qDebug() << "setEffectEnabled called:"
         << "type =" << static_cast<int>(effectType)
         << "enabled =" << enabled;

    for (auto &effect : effects)
    {

        if (effect->effectType() == effectType)
        {
            qDebug() << "setEffectEnabled match found:"
                     << "bypassed =" << effect->isBypassed();
            effect->setBypassed(!enabled);
            return;
        }
    }
}


void EffectChain::setEffectOrder(
    const QVector<AudioEffectType> &order)
{
    std::vector<std::unique_ptr<AudioEffect>> reordered;

    reordered.reserve(effects.size());

    for (AudioEffectType type : order)
    {
        for (auto &effect : effects)
        {
            if (effect && effect->effectType() == type)
            {
                reordered.push_back(std::move(effect));
                break;
            }
        }
    }

    // Keep any effects that weren't mentioned in the requested order.
    for (auto &effect : effects)
    {
        if (effect)
            reordered.push_back(std::move(effect));
    }

    effects = std::move(reordered);
}

void EffectChain::addEffect(AudioEffectType type)
{
    auto effect = AudioEffectFactory::create(type);

    if (!effect)
        return;

    effects.push_back(std::move(effect));
}

AudioEffect *EffectChain::findEffect(AudioEffectType type)
{
    for (auto &effect : effects)
    {
        if (effect->effectType() == type)
            return effect.get();
    }

    return nullptr;
}