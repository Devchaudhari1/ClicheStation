#include "StepGrid.h"

#include <QMouseEvent>
#include <QPainter>

namespace
{
    quint64 cellKey(int row, int step)
    {
        return (static_cast<quint64>(
                    static_cast<quint32>(row))
                << 32)
             | static_cast<quint32>(step);
    }
}

bool StepGrid::isCellActive(int row, int step) const
{
    return m_activeCells.contains(
        cellKey(row, step)
    );
}

void StepGrid::setCellActive(
    int row,
    int step,
    bool active)
{
    const quint64 key =
        cellKey(row, step);

    if (active)
        m_activeCells.insert(key);
    else
        m_activeCells.remove(key);
}

StepGrid::StepGrid(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

void StepGrid::setRowCount(int count)
{
    if (count < 1)
        count = 1;

    m_rowCount = count;

    updateGeometry();
    update();
}

void StepGrid::setStepCount(int count)
{
    if (count < 1)
        count = 1;

    m_stepCount = count;

    updateGeometry();
    update();
}

int StepGrid::rowCount() const
{
    return m_rowCount;
}

int StepGrid::stepCount() const
{
    return m_stepCount;
}

int StepGrid::cellWidth() const
{
    return m_cellWidth;
}

int StepGrid::cellHeight() const
{
    return m_cellHeight;
}

QSize StepGrid::sizeHint() const
{
    return QSize(
        m_stepCount * m_cellWidth,
        m_rowCount * m_cellHeight
    );
}

void StepGrid::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, false);

    const int width = m_stepCount * m_cellWidth;
    const int height = m_rowCount * m_cellHeight;

    // Background
    painter.fillRect(
        QRect(0, 0, width, height),
        palette().base()
    );

    // Grid
    for (int row = 0; row < m_rowCount; ++row)
    {
        for (int step = 0; step < m_stepCount; ++step)
        {
            const QRect cell(
                step * m_cellWidth,
                row * m_cellHeight,
                m_cellWidth,
                m_cellHeight
            );

            if (isCellActive(row, step))
            {
                painter.fillRect(
                    cell.adjusted(1, 1, -1, -1),
                    palette().highlight()
                );
            }

            painter.setPen(
                palette().mid().color()
            );

            painter.drawRect(cell);
        }
    }
}

void StepGrid::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    const QPoint position = event->position().toPoint();

    const int step =
        position.x() / m_cellWidth;

    const int row =
        position.y() / m_cellHeight;

    if (row < 0 ||
        row >= m_rowCount ||
        step < 0 ||
        step >= m_stepCount)
    {
        return;
    }

    const bool active =
        !isCellActive(row, step);

    setCellActive(
        row,
        step,
        active
    );

    update();

    emit cellClicked(row, step);
    emit patternChanged();
}