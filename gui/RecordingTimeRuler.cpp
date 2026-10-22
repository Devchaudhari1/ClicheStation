#include "RecordingTimeRuler.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QtGlobal>

RecordingTimeRuler::RecordingTimeRuler(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(30);
}

void RecordingTimeRuler::setDuration(double duration)
{
    m_duration = qMax(0.0, duration);

    const int totalWidth =
        static_cast<int>(m_duration * PixelsPerSecond);

    setFixedWidth(qMax(1, totalWidth));

    update();
}

void RecordingTimeRuler::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (m_duration <= 0.0)
        return;

    const double time =
        qBound(
            0.0,
            static_cast<double>(event->position().x()) / PixelsPerSecond,
            m_duration
        );

    emit positionClicked(time);
}

void RecordingTimeRuler::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    if (m_duration <= 0.0)
        return;

    painter.setPen(QPen());

    constexpr double majorStep = 1.0;

    for (double time = 0.0;
         time <= m_duration;
         time += majorStep)
    {
        const int x =
            static_cast<int>(time * PixelsPerSecond);

        painter.drawLine(
            x,
            height() - 9,
            x,
            height()
        );

        painter.drawText(
            x + 3,
            height() - 11,
            QString::number(time, 'f', 1) + " s"
        );
    }
}