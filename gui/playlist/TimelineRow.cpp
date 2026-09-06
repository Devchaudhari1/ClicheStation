#include "TimelineRow.h"

#include <QHBoxLayout>

TimelineRow::TimelineRow(QWidget *parent)
    : QWidget(parent),
      m_leftWidget(nullptr),
      m_timelineWidget(nullptr)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setFixedHeight(70);
}

QWidget *TimelineRow::leftWidget() const
{
    return m_leftWidget;
}

QWidget *TimelineRow::timelineWidget() const
{
    return m_timelineWidget;
}
void TimelineRow::setLeftWidget(QWidget *widget)
{
    if (!widget)
        return;

    m_leftWidget = widget;

    widget->setParent(this);

    widget->setFixedWidth(160);
    widget->setFixedHeight(70);

    QHBoxLayout *layout =
        qobject_cast<QHBoxLayout*>(
            this->layout()
        );

    if (!layout)
        return;

    layout->addWidget(
        widget
    );

    widget->show();
}

void TimelineRow::setTimelineWidget(QWidget *widget)
{
    if (!widget)
        return;

    m_timelineWidget = widget;

    widget->setParent(this);

    widget->setFixedHeight(70);

    QHBoxLayout *layout =
        qobject_cast<QHBoxLayout*>(
            this->layout()
        );

    if (!layout)
        return;

    layout->addWidget(
        widget,
        1
    );

    widget->show();
}