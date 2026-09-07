#pragma once

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QLabel;

class TimeRuler;

class Playlist : public QWidget
{
    Q_OBJECT

public:
    explicit Playlist(QWidget *parent = nullptr);

private:
    void createUI();

    // Main components
    TimeRuler *m_timeRuler;
    QWidget *m_timelineView;

    // Left side
    QScrollArea *m_leftPane;
    QWidget *m_leftContent;
    QVBoxLayout *m_leftLayout;

    // Right side
    QScrollArea *m_rightPane;
    QWidget *m_rightContent;
    QVBoxLayout *m_rightLayout;

    // Scroll synchronization
    bool m_syncingVerticalScroll = false;
};