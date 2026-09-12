#pragma once

#include "SynthesizersSamples.h"
#include "SampleData.h"
#include <QHash>
#include <QString>

class SampleBank
{
public:
    bool loadSample(
        int rootMidiNote,
        const QString& filePath
    );

    bool loadInstrument(SynthesizersSamples instrument);

    bool hasSample(int rootMidiNote) const;

    const SampleData* sample(int rootMidiNote) const;

private:
    QHash<int, SampleData> m_samples;
};