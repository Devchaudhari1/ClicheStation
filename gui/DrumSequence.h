#pragma once

#include <QWidget>
#include "../synth/DrumsetsSamples.h"
class QScrollArea;

class DrumGrid;
class DrumPad;
class AudioEngine;
class StepGrid;

struct DrumStepEvent
{
    DrumsetsSamples drum = DrumsetsSamples::None;
    int step = 0;
    int velocity = 127;
};

class DrumSequence : public QWidget
{
    Q_OBJECT

public:
    explicit DrumSequence(int trackId,AudioEngine* audioEngine,QWidget* parent = nullptr);
    ~DrumSequence() override = default;
    const QVector<DrumStepEvent>& drumSteps() const;
signals:
    void patternChanged();
    void drumPatternChanged(const QVector<DrumStepEvent>& steps);
private:
    void createUi();
    void createLayout();
    void createConnections();
    QVector<DrumStepEvent> m_drumSteps;
    int m_trackId;
    AudioEngine* m_audioEngine = nullptr;
    QScrollArea* m_verticalScrollArea = nullptr;

    DrumGrid* m_drumGrid = nullptr;
    StepGrid* m_stepGrid = nullptr;
    DrumPad* m_drumPad = nullptr;
};