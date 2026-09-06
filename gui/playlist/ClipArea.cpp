#include "ClipArea.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QPainter>
#include <QPen>

ClipArea::ClipArea(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);

    setMinimumWidth(2000);
    setMinimumHeight(70);
}

void ClipArea::dragEnterEvent(
    QDragEnterEvent *event
)
{
    if (event->mimeData()->hasFormat(
            "application/x-clichestation-track"
        ))
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void ClipArea::dropEvent(
    QDropEvent *event
)
{
    if (!event->mimeData()->hasFormat(
            "application/x-clichestation-track"
        ))
    {
        event->ignore();
        return;
    }

    QByteArray data =
        event->mimeData()->data(
            "application/x-clichestation-track"
        );

    bool ok = false;

    int trackId =
        data.toInt(&ok);

    if (!ok)
    {
        event->ignore();
        return;
    }

    QPoint position =
        event->position().toPoint();

    // position.x() is now relative to the
    // actual timeline area.
    double startTime =
        static_cast<double>(position.x())
        / m_pixelsPerSecond;

    if (startTime < 0.0)
        startTime = 0.0;

    qDebug()
        << "DROPPED TRACK"
        << trackId
        << "AT"
        << startTime
        << "SECONDS";

    emit trackDropped(
        trackId,
        startTime
    );

    event->acceptProposedAction();
}

void ClipArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // ---------------------------------------------
    // Vertical time grid
    // ---------------------------------------------

    QPen verticalPen;
    verticalPen.setColor(QColor(220, 220, 220));
    verticalPen.setStyle(Qt::DashLine);
    verticalPen.setWidth(1);

    painter.setPen(verticalPen);

    int firstSecond = 0;

    int lastSecond =
        static_cast<int>(
            width() / m_pixelsPerSecond
        ) + 1;

    for (int second = firstSecond;
         second <= lastSecond;
         ++second)
    {
        int x =
            static_cast<int>(
                second * m_pixelsPerSecond
            );

        painter.drawLine(
            x,
            0,
            x,
            height()
        );
    }

    // ---------------------------------------------
    // Horizontal Layer boundary
    // ---------------------------------------------

    QPen horizontalPen;
    horizontalPen.setColor(QColor(205, 205, 205));
    horizontalPen.setStyle(Qt::SolidLine);
    horizontalPen.setWidth(1);

    painter.setPen(horizontalPen);

    painter.drawLine(
        0,
        height() - 1,
        width(),
        height() - 1
    );
}