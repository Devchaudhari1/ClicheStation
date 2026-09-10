#pragma once

#include <QWidget>
#include <QPixmap>
#include <QVector>

class QLabel;

class ClipVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit ClipVisualizer(QWidget *parent = nullptr);

    // One-time creation of the waveform.
    void setSamples(
        const QVector<float>& samples,
        int fullWidth
    );

    // Full duration of the original rendered audio.
    void setDuration(double duration);

    // Changes only which part of the frozen waveform is visible.
    void setSourceRange(double startTime, double endTime);

    double duration() const;
    double sourceStart() const;
    double sourceEnd() const;

private:
    void createWaveformPixmap(const QVector<float> &samples, int fullWidth);

    QLabel *m_waveformLabel = nullptr;

    // Frozen waveform.
    QPixmap m_waveformPixmap;

    double m_duration = 0.0;
    double m_sourceStart = 0.0;
    double m_sourceEnd = 0.0;

    // Number of miniature bars used for the waveform.
    static constexpr int WaveformBars = 2048;
};