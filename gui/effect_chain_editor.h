#pragma once

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include "../audio/effects/AudioEffect.h"

class EffectSlot;

class EffectChainEditor : public QWidget
{
    Q_OBJECT

public:
    explicit EffectChainEditor(QWidget *parent = nullptr);

    void addEffect(AudioEffectType effectType);
    QVector<EffectSlot *> effectSlots() const;
    QVector<AudioEffectType> currentEffectOrder() const;

signals:
    void effectEnabledChanged(AudioEffectType effectType,
                          bool enabled);

void effectOrderChanged(const QVector<AudioEffectType> &order);

protected:

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    QPoint m_dragStartPosition;
    QVector<EffectSlot *> m_effectSlots;
};