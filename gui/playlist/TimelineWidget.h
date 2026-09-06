#pragma once

#include <QWidget>

class QDragEnterEvent;
class QDropEvent;

class TimelineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineWidget(QWidget *parent = nullptr);

signals:
    void trackDropped(int trackId, double startTime);

protected:
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    double m_pixelsPerSecond = 100.0;
    int m_durationSeconds = 20;
};