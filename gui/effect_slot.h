#pragma once

#include <QWidget>
#include <QString>
#include "../audio/effects/AudioEffect.h"

class QCheckBox;
class QLabel;

class EffectSlot : public QWidget
{
    Q_OBJECT

public:
    explicit EffectSlot(AudioEffectType effectType,
                        QWidget *parent = nullptr);

    QString effectName() const;
    AudioEffectType effectType() const;
    bool isEnabled() const;
    QString effectTypeName(AudioEffectType type);

    void setEnabled(bool enabled);

signals:
    void enabledChanged(bool enabled);
private:
    QString m_effectName;
    AudioEffectType m_effectType;
    QLabel *m_nameLabel;
    QCheckBox *m_enabledCheckBox;
};