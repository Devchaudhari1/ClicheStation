#include "TimeRuler.h"

#include <QPainter>

TimeRuler::TimeRuler(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(30);
}

void TimeRuler::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    const int rulerHeight = height();

    // Temporary:
    // first 160 px are reserved for the Layer column.
    const int layerColumnWidth = 220;

    painter.drawLine(
        layerColumnWidth,
        rulerHeight - 1,
        width(),
        rulerHeight - 1
    );

    for (int second = 0;
         second <= 2000;
         ++second)
    {
        int x =
        layerColumnWidth +
        static_cast<int>(
            second * m_pixelsPerSecond
        )
        - m_scrollOffset;

        painter.drawLine(
            x,
            rulerHeight - 10,
            x,
            rulerHeight
        );

        painter.drawText(
            x + 4,
            12,
            QString("%1s").arg(second)
        );
    }
}

void TimeRuler::setScrollOffset(int offset)
{
    m_scrollOffset = offset;
    update();
}