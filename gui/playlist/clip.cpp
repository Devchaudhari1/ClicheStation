#include "Clip.h"
#include "ClipVisualizer.h"

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double PixelsPerSecond = 100.0;
constexpr double MinimumClipDuration = 0.05;

constexpr int ResizeHandleWidth = 12;
}

Clip::Clip(
    int trackId,
    const QString& trackName,
    double startTime,
    double duration,
    QWidget* parent
)
    : QWidget(parent)
    , m_trackId(trackId)
    , m_trackName(trackName)
    , m_startTime(startTime)
    , m_duration(duration)
    , m_sourceStart(0.0)
    , m_sourceEnd(duration)
    , m_fullSourceDuration(duration)
{
    m_originalStartTime = m_startTime;
    m_originalDuration = m_duration;

    m_originalSourceStart = m_sourceStart;
    m_originalSourceEnd = m_sourceEnd;

    createUI();

    setMinimumHeight(50);
    setMouseTracking(false);
}

void Clip::createUI()
{
    m_trackLabel = new QLabel(m_trackName, this);

    m_visualizer = new ClipVisualizer(this);

    m_visualizer->setDuration(m_fullSourceDuration);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(m_trackLabel);
    layout->addWidget(m_visualizer, 1);

    setLayout(layout);
}

const QVector<float>& Clip::samples() const
{
    return m_samples;
}

void Clip::setSourceRange(double sourceStart, double sourceEnd)
{
    m_sourceStart = sourceStart;
    m_sourceEnd = sourceEnd;

    if (m_visualizer)
    {
        m_visualizer->setSourceRange(
            m_sourceStart,
            m_sourceEnd
        );
    }
}

void Clip::setSamples(
    const QVector<float>& samples,
    int fullWidth
)
{

    m_samples = samples;

    if (!m_visualizer)
        return;

    m_visualizer->setDuration(m_fullSourceDuration);

    m_visualizer->setSamples(
        samples,
        fullWidth
    );

    m_visualizer->setSourceRange(
        m_sourceStart,
        m_sourceEnd
    );
}

void Clip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = this->rect();
    rect.adjust(0.5, 0.5, -0.5, -0.5);

    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(QColor("#020911"));

    painter.drawRoundedRect(
        rect,
        5.0,
        5.0
    );

    // Left resize handle
    painter.setBrush(Qt::white);

    QRectF leftHandle(
        3,
        height() / 2.0 - 8,
        4,
        16
    );

    painter.drawRoundedRect(
        leftHandle,
        2,
        2
    );

    // Right resize handle
    QRectF rightHandle(
        width() - 7,
        height() / 2.0 - 8,
        4,
        16
    );

    painter.drawRoundedRect(
        rightHandle,
        2,
        2
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

double Clip::sourceStart() const
{
    return m_sourceStart;
}

double Clip::sourceEnd() const
{
    return m_sourceEnd;
}


double Clip::fullSourceDuration() const
{
    return m_fullSourceDuration;
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

void Clip::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    const int x = event->position().toPoint().x();

    m_originalStartTime = m_startTime;
    m_originalDuration = m_duration;

    m_originalSourceStart = m_sourceStart;
    m_originalSourceEnd = m_sourceEnd;

    m_dragStartPosition =
        event->globalPosition().toPoint();

    if (x <= ResizeHandleWidth)
    {
        m_resizing = true;
        m_resizeLeft = true;
        m_dragging = false;
    }
    else if (x >= width() - ResizeHandleWidth)
    {
        m_resizing = true;
        m_resizeLeft = false;
        m_dragging = false;
    }
    else
    {
        m_dragging = true;
        m_resizing = false;
    }

    event->accept();
}


void Clip::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_dragging && !m_resizing)
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    const QPoint currentGlobal =
        event->globalPosition().toPoint();

    const double deltaPixels =
        currentGlobal.x() - m_dragStartPosition.x();

    const double deltaTime =
        deltaPixels / PixelsPerSecond;

    // ---------------------------------------------------------
    // Moving the entire clip
    // ---------------------------------------------------------

    if (m_dragging)
    {
        const double newStartTime =
            std::max(
                0.0,
                m_originalStartTime + deltaTime
            );

        emit moveRequested(newStartTime);

        event->accept();
        return;
    }

    // ---------------------------------------------------------
    // Resizing
    // ---------------------------------------------------------

    if (m_resizeLeft)
    {
        double newSourceStart =
            m_originalSourceStart + deltaTime;

        newSourceStart = std::clamp(
            newSourceStart,
            0.0,
            m_originalSourceEnd - MinimumClipDuration
        );

        const double newSourceEnd =
            m_originalSourceEnd;

        const double newDuration =
            newSourceEnd - newSourceStart;

        const double newStartTime =
            m_originalStartTime + deltaTime;

        emit resizeRequested(
            true,
            newStartTime,
            newDuration,
            newSourceStart,
            newSourceEnd
        );

        event->accept();
        return;
    }

        // ---------------------------------------------------------
    // Right edge resize
    // ---------------------------------------------------------

        double newSourceStart =
            m_originalSourceStart;

        double newSourceEnd =
            m_originalSourceEnd + deltaTime;

        newSourceEnd = std::clamp(
            newSourceEnd,
            newSourceStart + MinimumClipDuration,
            m_fullSourceDuration
        );

        const double newDuration =
            newSourceEnd - newSourceStart;

        const double newStartTime =
            m_originalStartTime;

        emit resizeRequested(
            false,
            newStartTime,
            newDuration,
            newSourceStart,
            newSourceEnd
        );

        event->accept();
}

void Clip::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        m_resizing = false;
        m_resizeLeft = false;
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}