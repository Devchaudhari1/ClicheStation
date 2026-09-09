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

    createTrack(1);
    createTrack(2);
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

void AudioEngine::noteOn(int midiNote, int velocity)
{
    if (m_trackProcessors.empty())
        return;

    m_trackProcessors[0]->noteOn(midiNote, velocity);
}

void AudioEngine::noteOn(
    int trackId,
    int midiNote,
    int velocity
)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->noteOn(midiNote, velocity);
}

void AudioEngine::noteOff(int midiNote)
{
    if (m_trackProcessors.empty())
        return;

    m_trackProcessors[0]->noteOff(midiNote);
}

void AudioEngine::noteOff(
    int trackId,
    int midiNote
)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->noteOff(midiNote);
}

void AudioEngine::pitchBend(int channel, int value)
{
    if (m_trackProcessors.empty())
        return;

    m_trackProcessors[0]->pitchBend(channel, value);
}

void AudioEngine::render(float *left,
                         float *right,
                         std::size_t numSamples)
{
    if (m_trackProcessors.empty())
        return;

    m_trackProcessors[0]->render(left, right, numSamples);
}


double AudioEngine::sampleRate() const
{
    return m_sampleRate;
}

int AudioEngine::blockSize() const
{
    return m_blockSize;
}

TrackProcessor* AudioEngine::createTrack(int trackId)
{
    auto processor =
        std::make_unique<TrackProcessor>(trackId);

    processor->prepare(m_sampleRate, m_blockSize);

    TrackProcessor* result = processor.get();

    m_trackProcessors.push_back(std::move(processor));

    return result;
}

TrackProcessor* AudioEngine::findTrackProcessor(int trackId)
{
    for (auto& processor : m_trackProcessors)
    {
        if (processor->trackId() == trackId)
            return processor.get();
    }

    return nullptr;
}

void AudioEngine::setDistortionDrive(int trackId, float drive)
{
    TrackProcessor *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setDistortionDrive(drive);
}

// handles effectEnabledChanged signal from InstrumentSettigs->VoiceTrack->AudioEngine->Processor
void AudioEngine::setEffectEnabled(
    int trackId,
    AudioEffectType effectType,
    bool enabled)
{
    qDebug() << "AudioEngine::setEffectEnabled:"
             << "trackId =" << trackId
             << "type =" << static_cast<int>(effectType)
             << "enabled =" << enabled;

    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
    {
        qDebug() << "AudioEngine: TrackProcessor NOT FOUND";
        return;
    }

    qDebug() << "AudioEngine: TrackProcessor found";
    processor->setEffectEnabled(
        effectType,
        enabled
    );
}
// handles effectOrderChanged signal from InstrumentSettigs->VoiceTrack->AudioEngine->Processor

void AudioEngine::setEffectOrder(
    int trackId,
    const QVector<AudioEffectType> &order)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEffectOrder(order);
}

// set ClippingThreshold

void AudioEngine::setClippingThreshold(int trackId, float threshold)
{
    TrackProcessor *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setClippingThreshold(threshold);
}

//set DelayTime
void AudioEngine::setDelayTime(
    int trackId,
    double time)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setDelayTime(time);
}

//set DelayFeedback
void AudioEngine::setDelayFeedback(
    int trackId,
    double feedback)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setDelayFeedback(feedback);
}

//set DelayMix
void AudioEngine::setDelayMix(
    int trackId,
    double mix)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setDelayMix(mix);
}

//set ReverbRoomSize
void AudioEngine::setReverbRoomSize(
    int trackId,
    float roomSize)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setReverbRoomSize(roomSize);
}

//set ReverbDamping
void AudioEngine::setReverbDamping(
    int trackId,
    float damping)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setReverbDamping(damping);
}

//set ReverbWet
void AudioEngine::setReverbWet(
    int trackId,
    float wet)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setReverbWet(wet);
}

//set ReverbDecay
void AudioEngine::setReverbDecay(
    int trackId,
    float decay)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setReverbDecay(decay);
}

//set SaturationDrive
void AudioEngine::setSaturationDrive(
    int trackId,
    float drive)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setSaturationDrive(drive);
}

//set FlangerValues

void AudioEngine::setFlangerRate(int trackId, float rate)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setFlangerRate(rate);
}

void AudioEngine::setFlangerDepth(int trackId, float depth)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setFlangerDepth(depth);
}

void AudioEngine::setFlangerFeedback(int trackId, float feedback)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setFlangerFeedback(feedback);
}

void AudioEngine::setFlangerMix(int trackId, float mix)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setFlangerMix(mix);
}

//set Compression Values
void AudioEngine::setCompressionThreshold(
    int trackId,
    float threshold)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setCompressionThreshold(threshold);
}

void AudioEngine::setCompressionRatio(
    int trackId,
    float ratio)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setCompressionRatio(ratio);
}

void AudioEngine::setCompressionAttack(
    int trackId,
    float attack)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setCompressionAttack(attack);
}

void AudioEngine::setCompressionRelease(
    int trackId,
    float release)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setCompressionRelease(release);
}

void AudioEngine::setCompressionMakeupGain(
    int trackId,
    float gain)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setCompressionMakeupGain(gain);
}

//setFlanger values

void AudioEngine::setEQLowFrequency(int trackId, float frequency)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEQLowFrequency(frequency);
}

void AudioEngine::setEQLowGain(int trackId, float gain)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEQLowGain(gain);
}

void AudioEngine::setEQMidFrequency(int trackId, float frequency)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEQMidFrequency(frequency);
}

void AudioEngine::setEQMidGain(int trackId, float gain)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEQMidGain(gain);
}

void AudioEngine::setEQMidQ(int trackId, float q)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEQMidQ(q);
}

void AudioEngine::setEQHighFrequency(int trackId, float frequency)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEQHighFrequency(frequency);
}

void AudioEngine::setEQHighGain(int trackId, float gain)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setEQHighGain(gain);
}

void AudioEngine::setLimiterThreshold(
    int trackId,
    float threshold)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setLimiterThreshold(threshold);
}

void AudioEngine::setLimiterRelease(
    int trackId,
    float release)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setLimiterRelease(release);
}

void AudioEngine::setLimiterCeiling(
    int trackId,
    float ceiling)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setLimiterCeiling(ceiling);
}

void AudioEngine::setGateThreshold(
    int trackId,
    float threshold)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setGateThreshold(threshold);
}

void AudioEngine::setGateAttack(
    int trackId,
    float attack)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setGateAttack(attack);
}

void AudioEngine::setGateRelease(
    int trackId,
    float release)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setGateRelease(release);
}

void AudioEngine::setGateRange(
    int trackId,
    float range)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setGateRange(range);
}

void AudioEngine::setChorusRate(
    int trackId,
    float rate)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setChorusRate(rate);
}

void AudioEngine::setChorusDepth(
    int trackId,
    float depth)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setChorusDepth(depth);
}

void AudioEngine::setChorusMix(
    int trackId,
    float mix)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setChorusMix(mix);
}

void AudioEngine::setPhaserRate(
    int trackId,
    float rate)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setPhaserRate(rate);
}

void AudioEngine::setPhaserDepth(
    int trackId,
    float depth)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setPhaserDepth(depth);
}

void AudioEngine::setPhaserFeedback(
    int trackId,
    float feedback)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setPhaserFeedback(feedback);
}

void AudioEngine::setPhaserMix(
    int trackId,
    float mix)
{
    auto *processor = findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setPhaserMix(mix);
}