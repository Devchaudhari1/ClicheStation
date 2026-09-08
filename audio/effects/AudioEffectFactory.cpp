#include "AudioEffectFactory.h"

#include "Clipping.h"
#include "Delay.h"
#include "Distortion.h"
#include "Saturation.h"
#include "Reverb.h"

std::unique_ptr<AudioEffect>
AudioEffectFactory::create(AudioEffectType type)
{
    switch (type)
    {
    case AudioEffectType::Clipping:
        return std::make_unique<Clipping>();

    case AudioEffectType::Delay:
        return std::make_unique<Delay>();

    case AudioEffectType::Distortion:
        return std::make_unique<Distortion>();

    case AudioEffectType::Saturation:
        return std::make_unique<Saturation>();

    case AudioEffectType::Reverb:
        return std::make_unique<Reverb>();
    }

    return nullptr;
}