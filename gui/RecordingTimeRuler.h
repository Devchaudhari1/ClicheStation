#pragma once

#include <QWidget>

class QMouseEvent;
class QPaintEvent;

class RecordingTimeRuler : public QWidget
{
    Q_OBJECT

public:
    explicit RecordingTimeRuler(QWidget* parent = nullptr);

    void setDuration(double duration);

signals:
    void positionClicked(double time);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    static constexpr double PixelsPerSecond = 100.0;

    double m_duration = 0.0;
};