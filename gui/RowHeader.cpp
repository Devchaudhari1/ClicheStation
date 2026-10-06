#include "RowHeader.h"

#include <QMouseEvent>
#include <QPainter>

namespace
{
const QStringList drumNames =
{
    "Kick",
    "Snare",
    "HH Closed",
    "HH Open",
    "Hand Clap",
    "Tom 1",
    "Tom 2",
    "Tom 3",
    "Crash 1",
    "Crash 2",
    "Ride",
    "Ride Bell",
    "China",
    "Splash",
    "Stick Click",
    "Snare Rim",
    "Snare Sidestick",
    "HH Foot",
    "Crash 3",
    "Tom 1 Rim",
    "Tom 2 Rim",
    "Tom 3 Rim",
    "Snare Snares Off",
    "Snare Snares Off Rim",
    "Snare Snares Off Sidestick"
};
}

RowHeader::RowHeader(QWidget* parent)
    : QWidget(parent)
{
}

void RowHeader::setRowCount(int count)
{
    if (count < 1)
        count = 1;

    m_rowCount = count;

    updateGeometry();
    update();
}

int RowHeader::rowCount() const
{
    return m_rowCount;
}

int RowHeader::rowHeight() const
{
    return m_rowHeight;
}

QSize RowHeader::sizeHint() const
{
    return QSize(
        m_width,
        m_rowCount * m_rowHeight
    );
}

void RowHeader::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing,
        false
    );

    for (int row = 0; row < m_rowCount; ++row)
    {
        const QRect rect(
            0,
            row * m_rowHeight,
            m_width,
            m_rowHeight
        );

        painter.fillRect(
            rect,
            palette().base()
        );

        painter.setPen(
            palette().mid().color()
        );

        painter.drawRect(rect);

        if (row < drumNames.size())
        {
            painter.setPen(
                palette().text().color()
            );

            painter.drawText(
                rect.adjusted(8, 0, -8, 0),
                Qt::AlignVCenter | Qt::AlignLeft,
                drumNames.at(row)
            );
        }
    }
}

void RowHeader::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    const int row =
        event->position().y() / m_rowHeight;

    if (row < 0 || row >= m_rowCount)
        return;

    emit rowClicked(row);
}