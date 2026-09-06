#include "clip.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

Clip::Clip(
    int trackId,
    const QString &trackName,
    double startTime,
    double duration,
    QWidget *parent
)
    : QWidget(parent),
      m_trackId(trackId),
      m_trackName(trackName),
      m_startTime(startTime),
      m_duration(duration)
{
    setMinimumHeight(50);
}

void Clip::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    QRect clipRect =
        rect().adjusted(
            1,
            1,
            -1,
            -1
        );

    // Clip body
    painter.drawRect(clipRect);

    // Left edge
    painter.drawLine(
        0,
        0,
        0,
        height()
    );

    // Right edge
    painter.drawLine(
        width() - 1,
        0,
        width() - 1,
        height()
    );

    // Track name
    painter.drawText(
        QRect(
            10,
            6,
            width() - 20,
            20
        ),
        Qt::AlignLeft | Qt::AlignVCenter,
        m_trackName
    );

    // Time range
    QString timeRange =
        QString("%1s - %2s")
            .arg(QString::number(m_startTime, 'f', 2))
            .arg(QString::number(
                m_startTime + m_duration,
                'f',
                2
            ));

    painter.drawText(
        QRect(
            10,
            26,
            width() - 20,
            20
        ),
        Qt::AlignLeft | Qt::AlignVCenter,
        timeRange
    );
}

int Clip::trackId() const
{
    return m_trackId;
}

QString Clip::trackName() const
{
    return m_trackName;
}

double Clip::startTime() const
{
    return m_startTime;
}

double Clip::duration() const
{
    return m_duration;
}

void Clip::setStartTime(double startTime)
{
    m_startTime = startTime;
}

void Clip::setDuration(double duration)
{
    m_duration = duration;
}

QString Clip::endTimeText() const
{
    return QString::number(m_startTime + m_duration, 'f', 2);
}

void Clip::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        emit deleteRequested();
        event->accept();
        return;
    }

    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    const int edgeSize = 8;

    if (event->pos().x() <= edgeSize)
    {
        m_resizing = true;
        m_resizeLeft = true;
    }
    else if (event->pos().x() >= width() - edgeSize)
    {
        m_resizing = true;
        m_resizeLeft = false;
    }
    else
    {
        m_dragging = true;
    }

    m_dragStartPosition = event->pos();

    m_originalStartTime = m_startTime;
    m_originalDuration = m_duration;

    event->accept();
}

void Clip::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta =
        event->pos() - m_dragStartPosition;

    double deltaTime =
        static_cast<double>(delta.x()) / 100.0;

    // -------------------------
    // RESIZING
    // -------------------------
    if (m_resizing)
    {
        double newStartTime = m_originalStartTime;
        double newDuration = m_originalDuration;

        if (m_resizeLeft)
        {
            newStartTime =
                m_originalStartTime + deltaTime;

            double originalEnd =
                m_originalStartTime +
                m_originalDuration;

            newDuration =
                originalEnd - newStartTime;
        }
        else
        {
            newDuration =
                m_originalDuration + deltaTime;
        }

        const double minimumDuration = 0.1;

        if (newDuration < minimumDuration)
            return;

        if (newStartTime < 0.0)
            return;

        emit resizeRequested(
            m_resizeLeft,
            newStartTime,
            newDuration
        );

        event->accept();
        return;
    }

    // -------------------------
    // MOVING
    // -------------------------
    if (m_dragging)
    {
        double newStartTime =
            m_originalStartTime + deltaTime;

        if (newStartTime < 0.0)
            newStartTime = 0.0;

        emit moveRequested(newStartTime);

        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void Clip::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        m_resizing = false;
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}