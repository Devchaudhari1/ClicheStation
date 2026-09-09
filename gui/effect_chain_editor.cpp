#include "effect_chain_editor.h"
#include "effect_slot.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
EffectChainEditor::EffectChainEditor(QWidget *parent)
    : QWidget(parent),
      m_effectSlots()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    setAcceptDrops(true);
}

bool EffectChainEditor::eventFilter(QObject *watched, QEvent *event)
{
    EffectSlot *slot =
        qobject_cast<EffectSlot *>(watched);

    if (!slot)
        return QWidget::eventFilter(watched, event);

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent =
            static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::LeftButton)
        {
            m_dragStartPosition =
                mouseEvent->pos();
        }
    }

    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent =
            static_cast<QMouseEvent *>(event);

        if (!(mouseEvent->buttons() & Qt::LeftButton))
            return QWidget::eventFilter(watched, event);

        if ((mouseEvent->pos() - m_dragStartPosition)
                .manhattanLength()
            < QApplication::startDragDistance())
        {
            return QWidget::eventFilter(watched, event);
        }

        QDrag *drag = new QDrag(slot);

        QMimeData *mimeData = new QMimeData;

        mimeData->setData(
            "application/x-clichestation-effect-slot",
            QByteArray::number(
                reinterpret_cast<quintptr>(slot)
            )
        );

        drag->setMimeData(mimeData);

        drag->exec(Qt::MoveAction);

        return true;
    }

    return QWidget::eventFilter(watched, event);
}

void EffectChainEditor::addEffect(AudioEffectType effectType)
{
    EffectSlot *slot = new EffectSlot(effectType, this);

    slot->installEventFilter(this);
    m_effectSlots.append(slot);

    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->layout());

    if (!layout)
        return;

    layout->addWidget(slot);

    connect(
        slot,
        &EffectSlot::enabledChanged,
        this,
        [this, slot](bool enabled)
        {
            qDebug() << "EffectChainEditor:"
                 << "type =" << static_cast<int>(slot->effectType())
                 << "enabled =" << enabled;

            emit effectEnabledChanged(
                slot->effectType(),
                enabled
            );
        }
    );
}


QVector<AudioEffectType> EffectChainEditor::currentEffectOrder() const
{
    QVector<AudioEffectType> order;

    for (EffectSlot *slot : m_effectSlots)
        order.append(slot->effectType());

    return order;
}

QVector<EffectSlot *> EffectChainEditor::effectSlots() const
{
    return m_effectSlots;
}

void EffectChainEditor::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(
            "application/x-clichestation-effect-slot"))
    {
        event->acceptProposedAction();
    }
}

void EffectChainEditor::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(
            "application/x-clichestation-effect-slot"))
    {
        event->acceptProposedAction();
    }
}

void EffectChainEditor::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat(
            "application/x-clichestation-effect-slot"))
    {
        return;
    }

    QByteArray data =
        event->mimeData()->data(
            "application/x-clichestation-effect-slot");

    quintptr ptr = data.toULongLong();

    EffectSlot *draggedSlot =
        reinterpret_cast<EffectSlot *>(ptr);

    if (!draggedSlot)
        return;

    int fromIndex = m_effectSlots.indexOf(draggedSlot);

    if (fromIndex < 0)
        return;

    int toIndex = 0;

    QPoint dropPosition = event->position().toPoint();

    for (int i = 0; i < m_effectSlots.size(); ++i)
    {
        EffectSlot *slot = m_effectSlots[i];

        if (slot == draggedSlot)
            continue;

        int centerY =
            slot->geometry().center().y();

        if (dropPosition.y() > centerY)
            ++toIndex;
    }

    if (toIndex > m_effectSlots.size())
        toIndex = m_effectSlots.size();

    if (fromIndex < toIndex)
        --toIndex;

    if (fromIndex == toIndex)
    {
        event->acceptProposedAction();
        return;
    }

    m_effectSlots.move(fromIndex, toIndex);

    QVBoxLayout *layout =
        qobject_cast<QVBoxLayout *>(this->layout());

    if (!layout)
        return;

    layout->removeWidget(draggedSlot);

    layout->insertWidget(toIndex, draggedSlot);

    emit effectOrderChanged(currentEffectOrder());

    event->acceptProposedAction();
}

