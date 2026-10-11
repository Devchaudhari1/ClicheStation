#include "DrumSampleBank.h"
#include "DrumsetsSamples.h"

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

    std::int32_t readInt24(const QByteArray& data,int offset)
    {
        const auto* p =
            reinterpret_cast<const unsigned char*>(
                data.constData() + offset
            );

        std::int32_t value =




            static_cast<std::int32_t>(p[0])
            | (static_cast<std::int32_t>(p[1]) << 8)
            | (static_cast<std::int32_t>(p[2]) << 16);

        // Sign-extend 24-bit signed PCM to 32 bits.
        if (value & 0x00800000)
            value |= static_cast<std::int32_t>(0xFF000000);

        return value;
    }
}

bool DrumSampleBank::loadInstrument(
    DrumsetsSamples drum,
    int rootMidiNote)
{
    if (drum == DrumsetsSamples::None)
        return false;

    const QString filename =
        DrumsetsSampleFiles.value(drum);

    if (filename.isEmpty())
        return false;

    const QString samplePath =
        QDir(QCoreApplication::applicationDirPath())
            .filePath(
                "wav/Drumsets/" +
                filename +
                ".wav"
            );

    return loadSample(
        drum,
        rootMidiNote,
        samplePath
    );
}

bool DrumSampleBank::loadSample(
    DrumsetsSamples drum,
    int rootMidiNote,
    const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning()
            << "DrumSampleBank: Could not open"
            << filePath;

        return false;
    }

    const QByteArray data = file.readAll();

    if (data.size() < 44)
    {
        qWarning()
            << "DrumSampleBank: WAV file too small:"
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
            << "DrumSampleBank: Not a RIFF/WAVE file:"
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
                    << "DrumSampleBank: Invalid fmt chunk:"
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
            << "DrumSampleBank: Only PCM WAV is currently supported:"
            << filePath;

        return false;
    }

    if (bitsPerSample != 16 && bitsPerSample != 24)
    {
        qWarning()
            << "DrumSampleBank: Unsupported bit depth:"
            << bitsPerSample
            << "bits,"
            << "channels:"
            << numChannels
            << "sampleRate:"
            << sampleRate
            << "audioFormat:"
            << audioFormat
            << "file:"
            << filePath;

        return false;
    }

    if (numChannels != 1 && numChannels != 2)
    {
        qWarning()
            << "DrumSampleBank: Only mono/stereo WAV is currently supported:"
            << filePath;

        return false;
    }

    if (sampleRate == 0 || dataOffset < 0)
    {
        qWarning()
            << "DrumSampleBank: Invalid WAV data:"
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

        float leftValue = 0.0f;
        float rightValue = 0.0f;

        if (bitsPerSample == 16)
        {
            const std::int16_t left =
                readInt16(data, frameOffset);

            leftValue =
                static_cast<float>(left) / 32768.0f;

            rightValue = leftValue;

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
        }
        else if (bitsPerSample == 24)
        {
            const std::int32_t left =
                readInt24(data, frameOffset);

            leftValue =
                static_cast<float>(left) / 8388608.0f;

            rightValue = leftValue;

            if (numChannels == 2)
            {
                const std::int32_t right =
                    readInt24(
                        data,
                        frameOffset + bytesPerSample
                    );

                rightValue =
                    static_cast<float>(right) / 8388608.0f;
            }
        }

        sample.left[frame] = leftValue;
        sample.right[frame] = rightValue;
    }

    m_samples.insert(
        drum,
        std::move(sample)
    );

    qDebug()
        << "DrumSampleBank: Loaded"
        << filePath
        << "drum:"
        << static_cast<int>(drum)
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

bool DrumSampleBank::hasSample(DrumsetsSamples drum) const
{
    return m_samples.contains(drum);
}

const SampleData* DrumSampleBank::sample(DrumsetsSamples drum) const
{
    auto it =
        m_samples.constFind(drum);

    if (it == m_samples.constEnd())
        return nullptr;

    return &it.value();
}

bool DrumSampleBank::loadAll(int rootMidiNote)
{
    bool allLoaded = true;

    for (auto it = DrumsetsSampleFiles.constBegin();
         it != DrumsetsSampleFiles.constEnd();
         ++it)
    {
        const DrumsetsSamples drum = it.key();

        if (drum == DrumsetsSamples::None)
            continue;

        const bool loaded =
            loadInstrument(
                drum,
                rootMidiNote
            );

        if (!loaded)
            allLoaded = false;
    }

    return allLoaded;
}