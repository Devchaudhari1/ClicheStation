#include "AudioEngine.h"

#include <QMediaDevices>
#include <QAudioDevice>
#include <QDebug>
#include <QDataStream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cmath>
AudioOutputDevice::AudioOutputDevice(AudioEngine *engine)
    : QIODevice(engine),
    m_engine(engine)
{
}

qint64 AudioOutputDevice::bytesAvailable() const
{
    return 4096;
}
qint64 AudioOutputDevice::readData(char *data, qint64 maxlen)
{

    const int bytesPerSample = sizeof(float);
    const int channels = 2;
    const int bytesPerFrame = bytesPerSample * channels;

    if (maxlen < bytesPerFrame)
        return 0;

    qint64 bytesToWrite =
        maxlen - (maxlen % bytesPerFrame);

    std::size_t numFrames =
        static_cast<std::size_t>(bytesToWrite / bytesPerFrame);

    std::vector<float> left(numFrames);
    std::vector<float> right(numFrames);

    m_engine->render(
        left.data(),
        right.data(),
        numFrames
    );

    float *output =
        reinterpret_cast<float *>(data);

    for (std::size_t i = 0; i < numFrames; ++i)
    {
        output[i * 2] = left[i];
        output[i * 2 + 1] = right[i];
    }

    return bytesToWrite;
}

qint64 AudioOutputDevice::writeData(
    const char *data,
    qint64 len
)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return -1;
}

AudioEngine::AudioEngine(QObject* parent)
    : QObject(parent)
{
}

AudioEngine::~AudioEngine()
{
    stop();
}


bool AudioEngine::initialize()
{
    QAudioDevice device =QMediaDevices::defaultAudioOutput();
    qDebug() << "Audio device:";
    qDebug() << "  description:" << device.description();
    qDebug() << "  id:" << device.id();
    qDebug() << "  isNull:" << device.isNull();
    qDebug() << "  sample rate:" << device.preferredFormat().sampleRate();
    qDebug() << "  channels:" << device.preferredFormat().channelCount();
    qDebug() << "  format:" << device.preferredFormat().sampleFormat();
    if (device.isNull())
    {
        qWarning()
            << "No audio output device found";

        return false;
    }
    m_format.setSampleRate(
        static_cast<int>(m_sampleRate));

    m_format.setChannelCount(2);

    m_format.setSampleFormat(
        QAudioFormat::Float);

    if (!device.isFormatSupported(m_format))
    {
        qWarning()
            << "Audio format not supported";

        return false;
    }


    m_audioSink =
        new QAudioSink(
            device,
            m_format,
            this);
    connect(
        m_audioSink,
        &QAudioSink::stateChanged,
        this,
        [](QAudio::State state)
        {
            qDebug() << "AudioSink state changed:" << state;
        });

    m_synth.prepare(
        m_sampleRate,
        m_blockSize);

    m_effectChain.prepare(
        m_sampleRate,
        m_blockSize,
        2);

    m_outputDevice =new AudioOutputDevice(this);

    return true;
}

void AudioEngine::start()
{

    bool opened = m_outputDevice->open(QIODevice::ReadOnly);
    connect(
    m_audioSink,
    &QAudioSink::stateChanged,
    this,
    [](QAudio::State state)
    {
        qDebug() << "AudioSink state changed:" << state;
    });

    qDebug() << "output device opened:" << opened;
    qDebug() << "output device isOpen:" << m_outputDevice->isOpen();
    qDebug() << "output device isReadable:" << m_outputDevice->isReadable();

    m_audioSink->start(m_outputDevice);

    if (m_audioSink == nullptr)
    {
        qDebug() << "start(): m_audioSink is null";
        return;
    }

    if (m_running)
    {
        qDebug() << "start(): already running";
        return;
    }

    m_running = true;
}
void AudioEngine::stop()
{
    if (m_audioSink == nullptr)
        return;

    m_audioSink->stop();

    if (m_outputDevice)
        m_outputDevice->close();

    m_audioDevice = nullptr;

    m_running = false;
}
void AudioEngine::noteOn(
    int midiNote,
    int velocity)
{
    m_synth.noteOn(
        midiNote,
        velocity);
}

void AudioEngine::noteOff(
    int midiNote)
{
    m_synth.noteOff(
        midiNote);
}

void AudioEngine::pitchBend(
    int channel,
    int value)
{
    m_synth.pitchBend(
        channel,
        value);
}

void AudioEngine::render(
    float* left,
    float* right,
    std::size_t numSamples)
{
    m_synth.render(left, right, numSamples);
    m_effectChain.process(left, right, numSamples);
    for (std::size_t i = 0; i < numSamples; ++i)
    {
        if (!std::isfinite(left[i]) ||
            !std::isfinite(right[i]))
        {
            qWarning() << "INVALID AUDIO SAMPLE:"
                       << left[i]
                       << right[i];
            return;
        }
    }
}

double AudioEngine::sampleRate() const
{
    return m_sampleRate;
}

int AudioEngine::blockSize() const
{
    return m_blockSize;
}

SynthEngine& AudioEngine::synth()
{
    return m_synth;
}

EffectChain& AudioEngine::effects()
{
    return m_effectChain;
}