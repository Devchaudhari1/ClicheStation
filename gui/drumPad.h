#pragma once

#include <QWidget>
#include "../synth/DrumsetsSamples.h"

class DrumPad : public QWidget
{
    Q_OBJECT

public:
    explicit DrumPad(QWidget* parent = nullptr);
    ~DrumPad();

    DrumsetsSamples sampleForPad(int padIndex) const;

signals:
    void padPressed(int padIndex);

private:
    void createPad();
};