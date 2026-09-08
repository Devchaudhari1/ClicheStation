#include "ClipVisualizer.h"

#include <QPainter>
#include <QPaintEvent>
#include <QtGlobal>
#include <QSizePolicy>

ClipVisualizer::ClipVisualizer(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
}

void ClipVisualizer::setSamples(const QVector<float> &samples)
{
    m_samples = samples;
    update();
}

void ClipVisualizer::setDuration(double duration)
{
    m_duration = duration;
    update();
}

double ClipVisualizer::duration() const
{
    return m_duration;
}

void ClipVisualizer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, false);

    if (m_samples.isEmpty())
        return;

    const int width = this->width();
    const int height = this->height();

    const int centerY = height / 2;

    // Zero/amplitude axis
    painter.drawLine(
        0,
        centerY,
        width,
        centerY
    );

    /*
     * We don't necessarily draw every sample.
     *
     * If the clip contains 44100 samples and the widget
     * is only 800 pixels wide, drawing 44100 individual
     * bars would be wasteful.
     *
     * Instead, several samples can occupy one pixel column.
     */
    const int pixelWidth = qMax(1, width);

    for (int x = 0; x < pixelWidth; ++x)
    {
        int startSample =
            static_cast<int>(
                (static_cast<double>(x) / width)
                * m_samples.size()
            );

        int endSample =
            static_cast<int>(
                (static_cast<double>(x + 1) / width)
                * m_samples.size()
            );

        endSample = qMin(endSample, m_samples.size());

        if (startSample >= endSample)
            continue;

        float minValue = 1.0f;
        float maxValue = -1.0f;

        for (int i = startSample; i < endSample; ++i)
        {
            minValue = qMin(minValue, m_samples[i]);
            maxValue = qMax(maxValue, m_samples[i]);
        }

        int topY =
            centerY -
            static_cast<int>(maxValue * centerY);

        int bottomY =
            centerY -
            static_cast<int>(minValue * centerY);

        painter.drawLine(
            x,
            topY,
            x,
            bottomY
        );
    }
}