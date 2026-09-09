#include "TimelineWidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QPaintEvent>

TimelineWidget::TimelineWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(
    static_cast<int>(
        m_pixelsPerSecond * m_durationSeconds
    ),
    100);
}
void TimelineWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    const int rulerHeight = 30;

    // Ruler baseline
    painter.drawLine(
        0,
        rulerHeight,
        width(),
        rulerHeight
    );

    for (int second = 0;
         second <= m_durationSeconds;
         ++second)
    {
        int x =
            static_cast<int>(
                second * m_pixelsPerSecond
            );

        // Major tick
        painter.drawLine(
            x,
            rulerHeight - 8,
            x,
            rulerHeight
        );

        // Time label
        painter.drawText(
            x + 4,
            15,
            QString("%1s").arg(second)
        );
    }

    // Optional vertical grid lines
    for (int second = 0;
         second <= m_durationSeconds;
         ++second)
    {
        int x =
            static_cast<int>(
                second * m_pixelsPerSecond
            );

        painter.drawLine(
            x,
            rulerHeight,
            x,
            height()
        );
    }
}

void TimelineWidget::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "TIMELINE DRAG ENTER";

    if (event->mimeData()->hasFormat(
            "application/x-clichestation-track"))
    {
        qDebug() << "VALID TRACK DRAG";
        event->acceptProposedAction();
    }
    else
    {
        qDebug() << "INVALID DRAG";
        event->ignore();
    }
}
void TimelineWidget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat(
            "application/x-clichestation-track"))
    {
        event->ignore();
        return;
    }

    QByteArray data =
        event->mimeData()->data(
            "application/x-clichestation-track");

    bool ok = false;

    int trackId = data.toInt(&ok);

    if (!ok)
    {
        event->ignore();
        return;
    }

    double startTime =
        static_cast<double>(event->position().x())
        / m_pixelsPerSecond;

    emit trackDropped(trackId, startTime);

    event->acceptProposedAction();
}