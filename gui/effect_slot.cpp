#include "effect_slot.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>

EffectSlot::EffectSlot(AudioEffectType effectType, QWidget *parent)
    : QWidget(parent),
      m_effectType(effectType),
      m_nameLabel(nullptr),
      m_enabledCheckBox(nullptr)
{
    setFixedHeight(40);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(8);
    m_effectName= effectTypeName(m_effectType);
    m_nameLabel = new QLabel(m_effectName, this);
    m_enabledCheckBox = new QCheckBox(this);
    m_enabledCheckBox->setChecked(true);

    layout->addWidget(m_nameLabel);
    layout->addStretch();
    layout->addWidget(m_enabledCheckBox);

    connect(
        m_enabledCheckBox,
        &QCheckBox::toggled,
        this,
        &EffectSlot::enabledChanged
    );
}

QString EffectSlot::effectName() const
{
    return m_effectName;
}

bool EffectSlot::isEnabled() const
{
    return m_enabledCheckBox->isChecked();
}

void EffectSlot::setEnabled(bool enabled)
{
    m_enabledCheckBox->setChecked(enabled);
}

QString EffectSlot::effectTypeName(AudioEffectType type)
{
    switch (type)
    {
    case AudioEffectType::Clipping:
        return "Clipping";

    case AudioEffectType::Compression:
        return "Compression";

    case AudioEffectType::Delay:
        return "Delay";

    case AudioEffectType::Distortion:
        return "Distortion";

    case AudioEffectType::Flanger:
        return "Flanger";

    case AudioEffectType::Saturation:
        return "Saturation";

    case AudioEffectType::Reverb:
        return "Reverb";

    case AudioEffectType::EQ:
        return "EQ";

    case AudioEffectType::Limiter:
        return "Limiter";

    case AudioEffectType::Gate:
        return "Gate";

    case AudioEffectType::Chorus:
        return "Chorus";

    case AudioEffectType::Phaser:
        return "Phaser";
    }

    return "Unknown";
}

AudioEffectType EffectSlot::effectType() const
{
    return m_effectType;
}