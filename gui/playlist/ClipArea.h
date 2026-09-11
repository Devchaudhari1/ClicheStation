#pragma once

#include <QWidget>

class ClipArea : public QWidget
{
    Q_OBJECT

public:
    explicit ClipArea(QWidget *parent = nullptr);
    void ensureWidthForTime(double endTime);
signals:
    void trackDropped(int trackId, double startTime);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    double m_pixelsPerSecond = 100.0;
};