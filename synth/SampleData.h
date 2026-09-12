#pragma once

#include <QVector>

struct SampleData
{
    QVector<float> left;
    QVector<float> right;

    int sampleRate = 0;
    int rootMidiNote = 60;//C4

    bool isValid() const
    {
        return sampleRate > 0
            && rootMidiNote >= 0
            && !left.isEmpty()
            && left.size() == right.size();
    }

    std::size_t sampleCount() const
    {
        return static_cast<std::size_t>(left.size());
    }
};