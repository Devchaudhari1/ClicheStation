#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include "Sequence.h"
#include "TimeRuler.h"
class QScrollArea;
class QScrollBar;
class Sequence;

class TimelineView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineView(QWidget *parent = nullptr);

    void ensureWidthForTime(double endTime);
    void addSequence(Sequence *sequence);
    void layoutSequences();

private:
    QScrollArea *m_verticalScrollArea;
    QScrollArea *m_horizontalScrollArea;
    QWidget *m_timelineContent;

    QWidget *m_leftContent;
    QWidget *m_rightContent;

    QWidget *m_rightContainer;
    QWidget *m_leftTopPadding;
    QScrollBar *m_horizontalScrollBar;
    QScrollBar *m_nativeHorizontalScrollBar;
    
    QVector<Sequence*> m_sequences;

    QVector<QWidget*> m_leftSequenceContainers;
    QVector<QWidget*> m_rightSequenceContainers;

    QVector<QVBoxLayout*> m_leftSequenceLayouts;
    QVector<QVBoxLayout*> m_rightSequenceLayouts;

    TimeRuler *m_timeRuler;
    
    static constexpr int sequenceHeaderHeight = 60;
    static constexpr int layerHeight = 120;
};