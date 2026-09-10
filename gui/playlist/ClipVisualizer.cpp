#include "ClipVisualizer.h"

#include <QLabel>
#include <QImage>
#include <QVBoxLayout>
#include <QtGlobal>

#include <algorithm>
#include <cmath>

ClipVisualizer::ClipVisualizer(QWidget *parent)
    : QWidget(parent),
      m_waveformLabel(new QLabel(this))
{
    setContentsMargins(0, 0, 0, 0);

    m_waveformLabel->setContentsMargins(0, 0, 0, 0);
    m_waveformLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    /*
        The label contains the complete frozen waveform.

        The ClipVisualizer widget acts as the viewport.
        Anything outside this widget is automatically clipped.
    */
    m_waveformLabel->move(0, 0);
}

void ClipVisualizer::setSamples(
    const QVector<float>& samples,
    int fullWidth
)
{
    if (samples.isEmpty())
    {
        m_waveformPixmap = QPixmap();
        m_waveformLabel->clear();
        return;
    }

    /*
        This is the ONLY waveform construction step.
    */
    createWaveformPixmap(
        samples,
        fullWidth
    );

    m_waveformLabel->setPixmap(m_waveformPixmap);

    /*
        The label is intentionally larger than the viewport.
        The parent widget exposes only the required portion.
    */
    m_waveformLabel->setFixedSize(
        m_waveformPixmap.size()
    );

    setSourceRange(
        m_sourceStart,
        m_sourceEnd
    );
}

void ClipVisualizer::setDuration(double duration)
{
    m_duration = qMax(0.0, duration);

    if (m_sourceEnd <= 0.0)
        m_sourceEnd = m_duration;
}

void ClipVisualizer::setSourceRange(
    double startTime,
    double endTime)
{
    if (m_duration <= 0.0)
        return;

    if (m_waveformPixmap.isNull())
        return;

    m_sourceStart = qBound(
        0.0,
        startTime,
        m_duration
    );

    m_sourceEnd = qBound(
        m_sourceStart,
        endTime,
        m_duration
    );

    /*
        IMPORTANT:

        Nothing is rendered here.

        We simply move the existing waveform image so that
        sourceStart lines up with the left edge of the viewport.
    */

    const double startRatio =
        m_sourceStart / m_duration;

    const int startX =
        static_cast<int>(
            startRatio *
            m_waveformPixmap.width()
        );

    m_waveformLabel->move(
        -startX,
        0
    );
}

double ClipVisualizer::duration() const
{
    return m_duration;
}

double ClipVisualizer::sourceStart() const
{
    return m_sourceStart;
}

double ClipVisualizer::sourceEnd() const
{
    return m_sourceEnd;
}
void ClipVisualizer::createWaveformPixmap(
    const QVector<float>& samples,
    int fullWidth
)
{
    if (samples.isEmpty() || fullWidth <= 0)
        return;

    constexpr int height = 40;
    constexpr int maxBars = 2048;

    const int barCount =
        qMin(maxBars, fullWidth);

    QImage image(
        fullWidth,
        height,
        QImage::Format_ARGB32
    );

    image.fill(Qt::transparent);

    const QRgb waveformColor =
        qRgba(255, 255, 255, 255);

    const int centerY = height / 2;

    for (int bar = 0; bar < barCount; ++bar)
    {
        const int sampleStart =
            (bar * samples.size()) / barCount;

        const int sampleEnd =
            ((bar + 1) * samples.size()) / barCount;

        float peak = 0.0f;

        for (int i = sampleStart;
             i < sampleEnd;
             ++i)
        {
            peak = qMax(
                peak,
                std::abs(samples[i])
            );
        }

        const int barHeight =
            qMax(
                1,
                static_cast<int>(
                    peak * (height / 2 - 2)
                )
            );

        const int xStart =
            (bar * fullWidth) / barCount;

        const int xEnd =
            ((bar + 1) * fullWidth) / barCount;

        for (int x = xStart;
             x < qMax(xStart + 1, xEnd);
             ++x)
        {
            for (int y = centerY - barHeight;
                 y <= centerY + barHeight;
                 ++y)
            {
                if (y >= 0 && y < height)
                    image.setPixel(
                        x,
                        y,
                        waveformColor
                    );
            }
        }
    }

    m_waveformPixmap =
        QPixmap::fromImage(image);
}