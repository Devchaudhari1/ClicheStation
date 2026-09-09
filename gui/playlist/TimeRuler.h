#pragma once

#include <QWidget>

class TimeRuler : public QWidget
{
    Q_OBJECT

public:
    explicit TimeRuler(QWidget *parent = nullptr);
    void setScrollOffset(int offset);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_pixelsPerSecond = 100.0;
    int m_scrollOffset = 0;
};