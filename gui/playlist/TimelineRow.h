#pragma once

#include <QWidget>

class TimelineRow : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineRow(QWidget *parent = nullptr);

    QWidget *leftWidget() const;
    QWidget *timelineWidget() const;

    void setLeftWidget(QWidget *widget);
    void setTimelineWidget(QWidget *widget);

private:
    QWidget *m_leftWidget;
    QWidget *m_timelineWidget;
};