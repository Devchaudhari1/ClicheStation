#include "SampleBank.h"
#include "SynthesizersSamples.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

#include <cstdint>
#include <cstring>

namespace
{
    std::uint16_t readUInt16(const QByteArray& data, int offset)
    {
        const auto *p =
            reinterpret_cast<const unsigned char*>(data.constData() + offset);

        return static_cast<std::uint16_t>(p[0])
             | (static_cast<std::uint16_t>(p[1]) << 8);
    }

    std::uint32_t readUInt32(const QByteArray& data, int offset)
    {
        const auto *p =
            reinterpret_cast<const unsigned char*>(data.constData() + offset);

        return static_cast<std::uint32_t>(p[0])
             | (static_cast<std::uint32_t>(p[1]) << 8)
             | (static_cast<std::uint32_t>(p[2]) << 16)
             | (static_cast<std::uint32_t>(p[3]) << 24);
    }

    std::int16_t readInt16(const QByteArray& data, int offset)
    {
        return static_cast<std::int16_t>(
            readUInt16(data, offset)
        );
    }
}

bool SampleBank::loadInstrument(SynthesizersSamples instrument)
{
    if (instrument == SynthesizersSamples::None)
        return false;

    const QString filename =
        SynthesizersSampleFiles.value(instrument);

    if (filename.isEmpty())
        return false;

    const QString samplePath =
        QDir(QCoreApplication::applicationDirPath())
            .filePath(
                "wav/Synthesizers/" +
                filename +
                ".wav"
            );

    return loadSample(
        60,        // C4
        samplePath
    );
}

bool SampleBank::loadSample(
    int rootMidiNote,
    const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning()
            << "SampleBank: Could not open"
            << filePath;

        return false;
    }

    const QByteArray data = file.readAll();

    if (data.size() < 44)
    {
        qWarning()
            << "SampleBank: WAV file too small:"
            << filePath;

        return false;
    }

    /*
        RIFF header
    */

    if (data.mid(0, 4) != "RIFF"
        || data.mid(8, 4) != "WAVE")
    {
        qWarning()
            << "SampleBank: Not a RIFF/WAVE file:"
            << filePath;

        return false;
    }

    int offset = 12;

    std::uint16_t audioFormat = 0;
    std::uint16_t numChannels = 0;
    std::uint32_t sampleRate = 0;
    std::uint16_t bitsPerSample = 0;

    int dataOffset = -1;
    std::uint32_t dataSize = 0;

    /*
        WAV consists of chunks.

        We look for:

            fmt
            data
    */

    while (offset + 8 <= data.size())
    {
        const QByteArray chunkId =
            data.mid(offset, 4);

        const std::uint32_t chunkSize =
            readUInt32(data, offset + 4);

        const int chunkDataOffset =
            offset + 8;

        if (chunkDataOffset + chunkSize > data.size())
            break;

        if (chunkId == "fmt ")
        {
            if (chunkSize < 16)
            {
                qWarning()
                    << "SampleBank: Invalid fmt chunk:"
                    << filePath;

                return false;
            }

            audioFormat =
                readUInt16(data, chunkDataOffset);

            numChannels =
                readUInt16(data, chunkDataOffset + 2);

            sampleRate =
                readUInt32(data, chunkDataOffset + 4);

            bitsPerSample =
                readUInt16(data, chunkDataOffset + 14);
        }
        else if (chunkId == "data")
        {
            dataOffset = chunkDataOffset;
            dataSize = chunkSize;
            break;
        }

        offset += 8 + static_cast<int>(chunkSize);

        // WAV chunks are word-aligned.
        if (chunkSize & 1)
            ++offset;
    }

    /*
        For our first implementation we support:

            PCM
            16-bit
            mono or stereo
    */

    if (audioFormat != 1)
    {
        qWarning()
            << "SampleBank: Only PCM WAV is currently supported:"
            << filePath;

        return false;
    }

    if (bitsPerSample != 16)
    {
        qWarning()
            << "SampleBank: Only 16-bit WAV is currently supported:"
            << filePath;

        return false;
    }

    if (numChannels != 1 && numChannels != 2)
    {
        qWarning()
            << "SampleBank: Only mono/stereo WAV is currently supported:"
            << filePath;

        return false;
    }

    if (sampleRate == 0 || dataOffset < 0)
    {
        qWarning()
            << "SampleBank: Invalid WAV data:"
            << filePath;

        return false;
    }

    const int bytesPerSample =
        static_cast<int>(bitsPerSample / 8);

    const int frameSize =
        bytesPerSample * numChannels;

    const int frameCount =
        static_cast<int>(dataSize / frameSize);

    SampleData sample;

    sample.sampleRate =
        static_cast<int>(sampleRate);

    sample.rootMidiNote =
        rootMidiNote;

    sample.left.resize(frameCount);
    sample.right.resize(frameCount);

    for (int frame = 0; frame < frameCount; ++frame)
    {
        const int frameOffset =
            dataOffset + frame * frameSize;

        const std::int16_t left =
            readInt16(data, frameOffset);

        float leftValue =
            static_cast<float>(left) / 32768.0f;

        float rightValue = leftValue;

        if (numChannels == 2)
        {
            const std::int16_t right =
                readInt16(
                    data,
                    frameOffset + bytesPerSample
                );

            rightValue =
                static_cast<float>(right) / 32768.0f;
        }

        sample.left[frame] = leftValue;
        sample.right[frame] = rightValue;
    }

    m_samples.insert(rootMidiNote, std::move(sample));

    qDebug()
        << "SampleBank: Loaded"
        << filePath
        << "root MIDI:"
        << rootMidiNote
        << "sample rate:"
        << sampleRate
        << "frames:"
        << frameCount
        << "channels:"
        << numChannels;

    return true;
}

bool SampleBank::hasSample(int rootMidiNote) const
{
    return m_samples.contains(rootMidiNote);
}

const SampleData* SampleBank::sample(int rootMidiNote) const
{
    auto it = m_samples.constFind(rootMidiNote);

    if (it == m_samples.constEnd())
        return nullptr;

    return &it.value();
}