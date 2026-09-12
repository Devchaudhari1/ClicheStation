#pragma once

#include <QWidget>

class QScrollArea;
class QWidget;

class TimelineView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineView(QWidget *parent = nullptr);
    void ensureWidthForTime(double endTime);
private:
    QScrollArea *m_leftPane;
    QScrollArea *m_rightPane;

    QWidget *m_leftContent;
    QWidget *m_rightContent;
};