#pragma once

#include "DrumsetsSamples.h"
#include "SampleData.h"

#include <QHash>

class DrumSampleBank
{
public:
    bool loadSample(
        DrumsetsSamples drum,
        int rootMidiNote,
        const QString& filePath
    );

    bool loadInstrument(
        DrumsetsSamples drum,
        int rootMidiNote
    );

    bool hasSample(DrumsetsSamples drum) const;

    const SampleData* sample(DrumsetsSamples drum) const;
    bool loadAll(int rootMidiNote);
private:
    QHash<DrumsetsSamples, SampleData> m_samples;
};