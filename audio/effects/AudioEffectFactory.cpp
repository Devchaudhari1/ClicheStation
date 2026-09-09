#include "AudioEffectFactory.h"

#include "Chorus.h"
#include "Clipping.h"
#include "Compression.h"
#include "Delay.h"
#include "Distortion.h"
#include "EQ.h"
#include "Flanger.h"
#include "Gate.h"
#include "Limiter.h"
#include "Phaser.h"
#include "Reverb.h"
#include "Saturation.h"

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

    case AudioEffectType::Flanger:
        return std::make_unique<Flanger>();

    case AudioEffectType::Compression:
        return std::make_unique<Compression>();

    case AudioEffectType::Limiter:
        return std::make_unique<Limiter>();

    case AudioEffectType::EQ:
        return std::make_unique<EQ>();
        
    case AudioEffectType::Gate:
        return std::make_unique<Gate>();

    case AudioEffectType::Chorus:
        return std::make_unique<Chorus>();

    case AudioEffectType::Phaser:
        return std::make_unique<Phaser>();
    }
    

return nullptr;
}