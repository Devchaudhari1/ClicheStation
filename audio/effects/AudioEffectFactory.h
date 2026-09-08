#pragma once

#include "AudioEffect.h"

#include <memory>

class AudioEffectFactory
{
public:
    static std::unique_ptr<AudioEffect> create(AudioEffectType type);
};