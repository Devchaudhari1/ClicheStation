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

AudioInputDevice::AudioInputDevice(AudioEngine *engine)
    : QIODevice(engine),
      m_engine(engine)
{
}

qint64 AudioInputDevice::bytesAvailable() const
{
    return QIODevice::bytesAvailable();
}

qint64 AudioInputDevice::readData(
    char *data,
    qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    return 0;
}

qint64 AudioInputDevice::writeData(
    const char *data,
    qint64 len)
{
    if (!m_engine || !data || len <= 0)
        return 0;

    const int bytesPerSample = sizeof(float);
    const int channels = 2;
    const int bytesPerFrame =
        bytesPerSample * channels;

    if (len < bytesPerFrame)
        return 0;

    const qint64 bytesToProcess =
        len - (len % bytesPerFrame);

    const std::size_t numFrames =
        static_cast<std::size_t>(
            bytesToProcess / bytesPerFrame);

    const float *input =
        reinterpret_cast<const float *>(data);

    std::vector<float> left(numFrames);
    std::vector<float> right(numFrames);

    for (std::size_t i = 0; i < numFrames; ++i)
    {
        left[i] = input[i * 2];
        right[i] = input[i * 2 + 1];
    }

    m_engine->processInput(
        left.data(),
        right.data(),
        numFrames
    );

    return bytesToProcess;
}

AudioEngine::AudioEngine(QObject* parent)
    : QObject(parent)
{
}

AudioEngine::~AudioEngine()
{
    stop();
}

void AudioEngine::startSequence(
    const QVector<SequenceClipPlayback>& clips)
{
    m_sequenceClips = clips;

    m_sequencePlaybackPosition.store(
        0,
        std::memory_order_relaxed
    );

    m_sequencePlaying.store(
        true,
        std::memory_order_release
    );
}

void AudioEngine::stopSequence()
{
    m_sequencePlaying.store(
        false,
        std::memory_order_release
    );

    m_sequencePlaybackPosition.store(
        0,
        std::memory_order_relaxed
    );
}

bool AudioEngine::isSequencePlaying() const
{
    return m_sequencePlaying.load(
        std::memory_order_acquire
    );
}

std::int64_t AudioEngine::sequencePlaybackPosition() const
{
    return m_sequencePlaybackPosition.load(
        std::memory_order_relaxed
    );
}

void AudioEngine::renderSequence(
    float* left,
    float* right,
    std::size_t numSamples)
{
    if (!m_sequencePlaying.load(
            std::memory_order_acquire))
    {
        return;
    }

    const std::int64_t sequencePosition =
        m_sequencePlaybackPosition.load(
            std::memory_order_relaxed);

    const std::int64_t blockStart =
        sequencePosition;

    const std::int64_t blockEnd =
        blockStart +
        static_cast<std::int64_t>(numSamples);

    for (const SequenceClipPlayback& clip :
         m_sequenceClips)
    {
        if (!clip.samples)
            continue;

        const std::int64_t clipStart =
            clip.startSample;

        const std::int64_t clipEnd =
            clip.startSample +
            (clip.sourceEndSample -
             clip.sourceStartSample);

        // No overlap with this audio block.
        if (blockEnd <= clipStart ||
            blockStart >= clipEnd)
        {
            continue;
        }

        const std::int64_t overlapStart =
            std::max(
                blockStart,
                clipStart
            );

        const std::int64_t overlapEnd =
            std::min(
                blockEnd,
                clipEnd
            );

        const std::int64_t sourceOffset =
            clip.sourceStartSample +
            (overlapStart - clipStart);

        const std::int64_t outputOffset =
            overlapStart - blockStart;

        const std::int64_t count =
            overlapEnd - overlapStart;

        for (std::int64_t i = 0; i < count; ++i)
        {
            const std::int64_t sourceIndex =
                sourceOffset + i;

            const std::int64_t outputIndex =
                outputOffset + i;

            if (sourceIndex < 0 ||
                sourceIndex >= clip.samples->size())
            {
                continue;
            }

            const float sample =
                (*clip.samples)[sourceIndex];

            left[outputIndex] += sample;
            right[outputIndex] += sample;
        }
    }

    m_sequencePlaybackPosition.store(
        blockEnd,
        std::memory_order_relaxed
    );
}

bool AudioEngine::initialize()
{
    QAudioDevice device =QMediaDevices::defaultAudioOutput();
    // qDebug() << "Audio device:";
    // qDebug() << "  description:" << device.description();
    // qDebug() << "  id:" << device.id();
    // qDebug() << "  isNull:" << device.isNull();
    // qDebug() << "  sample rate:" << device.preferredFormat().sampleRate();
    // qDebug() << "  channels:" << device.preferredFormat().channelCount();
    // qDebug() << "  format:" << device.preferredFormat().sampleFormat();
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

    QAudioDevice inputDevice =
        QMediaDevices::defaultAudioInput();

    if (inputDevice.isNull())
    {
        qWarning()
            << "No audio input device found";

        return false;
    }

    QAudioFormat inputFormat;
    inputFormat.setSampleRate(
        static_cast<int>(m_sampleRate));

    inputFormat.setChannelCount(2);

    inputFormat.setSampleFormat(
        QAudioFormat::Float);

    if (!inputDevice.isFormatSupported(inputFormat))
    {
        qWarning()
            << "Audio input format not supported";

        return false;
    }

    m_audioSource =
        new QAudioSource(
            inputDevice,
            inputFormat,
            this);

    connect(
        m_audioSource,
        &QAudioSource::stateChanged,
        this,
        [](QAudio::State state)
        {
            qDebug()
                << "AudioSource state changed:"
                << state;
    });

    createTrack(1);
    createTrack(2);
    m_outputDevice =new AudioOutputDevice(this);
    m_inputDevice = new AudioInputDevice(this);
    m_realTimeSampler.prepare(
        m_sampleRate,
        static_cast<std::size_t>(m_blockSize),
        2,
        60.0
    );
    return true;
}

void AudioEngine::setSampleInstrument(
    int trackId,
    SynthesizersSamples sample)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setSampleInstrument(sample);
}

void AudioEngine::start()
{
    if (m_audioSink == nullptr)
    {
        qDebug()
            << "start(): m_audioSink is null";
        return;
    }

    if (m_audioSource == nullptr)
    {
        qDebug()
            << "start(): m_audioSource is null";
        return;
    }

    if (m_outputDevice == nullptr)
    {
        qDebug()
            << "start(): m_outputDevice is null";
        return;
    }

    if (m_inputDevice == nullptr)
    {
        qDebug()
            << "start(): m_inputDevice is null";
        return;
    }

    if (m_running)
    {
        qDebug()
            << "start(): already running";
        return;
    }

    bool opened =
        m_outputDevice->open(QIODevice::ReadOnly);

    qDebug()
        << "output device opened:"
        << opened;

    qDebug()
        << "output device isOpen:"
        << m_outputDevice->isOpen();

    qDebug()
        << "output device isReadable:"
        << m_outputDevice->isReadable();

    m_audioSink->start(m_outputDevice);

    bool inputOpened =
        m_inputDevice->open(QIODevice::WriteOnly);

    qDebug()
        << "input device opened:"
        << inputOpened;

    qDebug()
        << "input device isOpen:"
        << m_inputDevice->isOpen();

    m_audioSource->start(m_inputDevice);

    m_running = true;
}

void AudioEngine::stop()
{
    if (m_audioSink == nullptr)
        return;

    m_audioSink->stop();

    if (m_audioSource)
        m_audioSource->stop();

    if (m_outputDevice)
        m_outputDevice->close();

    if (m_inputDevice)
        m_inputDevice->close();

    m_audioDevice = nullptr;

    m_running = false;
}

void AudioEngine::noteOn(int midiNote, int velocity)
{
    if (m_trackProcessors.empty())
        return;
    m_trackProcessors[0]->noteOn(midiNote, velocity);
}

void AudioEngine::processInput(
    const float *left,
    const float *right,
    std::size_t numSamples)
{
    if (!left || !right || numSamples == 0)
        return;

    m_realTimeSampler.process(
        left,
        right,
        numSamples
    );
}

void AudioEngine::startRecording()
{
    m_realTimeSampler.startRecording();
}

void AudioEngine::stopRecording()
{
    m_realTimeSampler.stopRecording();

    qDebug()
        << "Voice recording stopped."
        << "Samples:"
        << m_realTimeSampler.numSamples();

    qDebug()
        << "Duration:"
        << (m_realTimeSampler.numSamples()
            / m_realTimeSampler.sampleRate())
        << "seconds";
    emit recordingStopped();
}

void AudioEngine::trimRecording(
    std::size_t startSample,
    std::size_t endSample)
{
    m_realTimeSampler.trim(
        startSample,
        endSample);
}

const std::vector<float>& AudioEngine::recordedLeft() const
{
    return m_realTimeSampler.leftBuffer();
}

const std::vector<float>& AudioEngine::recordedRight() const
{
    return m_realTimeSampler.rightBuffer();
}

std::size_t AudioEngine::recordedSamples() const
{
    return m_realTimeSampler.numSamples();
}

void AudioEngine::startRecordingPlayback(
    std::size_t startSample)
{
    const std::size_t totalSamples =
        m_realTimeSampler.numSamples();

    if (totalSamples == 0)
        return;

    if (startSample >= totalSamples)
        startSample = 0;

    m_recordingPlaybackPosition.store(
        startSample,
        std::memory_order_relaxed
    );

    m_recordingPlayback.store(
        true,
        std::memory_order_release
    );
}

void AudioEngine::stopRecordingPlayback()
{
    m_recordingPlayback.store(
        false,
        std::memory_order_release
    );
}

bool AudioEngine::isRecordingPlayback() const
{
    return m_recordingPlayback.load(
        std::memory_order_acquire
    );
}

void AudioEngine::noteOn(
    int trackId,
    int midiNote,
    int velocity
)
{
    TrackProcessor *processor =
        findTrackProcessor(trackId);
        qDebug() << "noteOn:"
            << "trackId =" << trackId
            << "processor =" << processor;
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

void AudioEngine::drumNoteOn(
    int trackId,
    DrumsetsSamples drum,
    int midiNote,
    int velocity)
{
    qDebug()
        << "[AudioEngine]"
        << "drumNoteOn:"
        << "track =" << trackId
        << "drum =" << static_cast<int>(drum)
        << "note =" << midiNote
        << "velocity =" << velocity;

    TrackProcessor* processor =
        findTrackProcessor(trackId);

    if (!processor)
    {
        qDebug()
            << "[AudioEngine]"
            << "ERROR: no TrackProcessor for track:"
            << trackId;

        return;
    }

    qDebug()
        << "[AudioEngine]"
        << "forwarding to TrackProcessor";

    processor->drumNoteOn(
        drum,
        midiNote,
        velocity
    );
}

void AudioEngine::setDrumInstrument(
    int trackId,
    DrumsetsSamples drum,
    int rootMidiNote)
{
    TrackProcessor* processor =
        findTrackProcessor(trackId);

    if (!processor)
        return;

    processor->setDrumInstrument(
        drum,
        rootMidiNote
    );
}

void AudioEngine::setTrackMuted(int trackId, bool muted)
{
    m_trackRoutingStates[trackId].muted = muted;
}

bool AudioEngine::isTrackMuted(int trackId) const
{
    auto it = m_trackRoutingStates.constFind(trackId);

    if (it == m_trackRoutingStates.constEnd())
        return false;

    return it->muted;
}

void AudioEngine::setTrackSoloed(int trackId, bool soloed)
{
    m_trackRoutingStates[trackId].soloed = soloed;
}

bool AudioEngine::isTrackSoloed(int trackId) const
{
    auto it = m_trackRoutingStates.constFind(trackId);

    if (it == m_trackRoutingStates.constEnd())
        return false;

    return it->soloed;
}

bool AudioEngine::shouldTrackParticipate(int trackId) const
{
    const TrackProcessor *processor = findTrackProcessor(trackId);

    if (!processor)
        return false;

    if (processor->state() == TrackState::Inactive)
        return false;

    const bool muted = isTrackMuted(trackId);

    bool anySolo = false;

    for (const auto& processorPtr : m_trackProcessors)
    {
        const int otherTrackId = processorPtr->trackId();

        if (isTrackSoloed(otherTrackId))
        {
            anySolo = true;
            break;
        }
    }

    if (anySolo)
        return isTrackSoloed(trackId) && !muted;

    return !muted;
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
    std::fill(left, left + numSamples, 0.0f);
    std::fill(right, right + numSamples, 0.0f);

    std::vector<float> trackLeft(numSamples);
    std::vector<float> trackRight(numSamples);

    for (auto& processor : m_trackProcessors)
    {
        if (!shouldTrackParticipate(processor->trackId()))
            continue;

        std::fill(
            trackLeft.begin(),
            trackLeft.end(),
            0.0f
        );

        std::fill(
            trackRight.begin(),
            trackRight.end(),
            0.0f
        );

        processor->render(
            trackLeft.data(),
            trackRight.data(),
            numSamples
        );

        for (std::size_t i = 0;
             i < numSamples;
             ++i)
        {
            left[i] += trackLeft[i];
            right[i] += trackRight[i];
        }
    }

    // --------------------------------------------
    // Sequence playback
    // --------------------------------------------

    renderSequence(
        left,
        right,
        numSamples
    );

    // --------------------------------------------
    // Recording playback
    // --------------------------------------------

    if (m_recordingPlayback.load(
            std::memory_order_acquire))
    {
        const std::size_t position =
            m_recordingPlaybackPosition.load(
                std::memory_order_relaxed
            );

        const std::size_t totalSamples =
            m_realTimeSampler.numSamples();

        if (position < totalSamples)
        {
            const std::vector<float>& recordingLeft =
                m_realTimeSampler.leftBuffer();

            const std::vector<float>& recordingRight =
                m_realTimeSampler.rightBuffer();

            const std::size_t samplesToRender =
                std::min(
                    numSamples,
                    totalSamples - position
                );

            for (std::size_t i = 0;
                 i < samplesToRender;
                 ++i)
            {
                left[i] += recordingLeft[position + i];
                right[i] += recordingRight[position + i];
            }

            const std::size_t newPosition =
                position + samplesToRender;

            if (newPosition >= totalSamples)
            {
                m_recordingPlaybackPosition.store(
                    0,
                    std::memory_order_relaxed
                );

                m_recordingPlayback.store(
                    false,
                    std::memory_order_release
                );
            }
            else
            {
                m_recordingPlaybackPosition.store(
                    newPosition,
                    std::memory_order_relaxed
                );
            }
        }
        else
        {
            m_recordingPlaybackPosition.store(
                0,
                std::memory_order_relaxed
            );

            m_recordingPlayback.store(
                false,
                std::memory_order_release
            );
        }
    }
}

QVector<float> AudioEngine::renderClip(
    int trackId,
    const QVector<PlacedNote>& notes)
{
    TrackProcessor* processor =
        findTrackProcessor(trackId);

    if (!processor)
        return {};

    return processor->renderClip(notes);
}

QVector<float> AudioEngine::renderClip(
    int trackId,
    const QVector<DrumStepEvent>& drumSteps,
    double secondsPerStep)
{
    TrackProcessor* processor =
        findTrackProcessor(trackId);

    if (!processor)
        return {};

    return processor->renderClip(
        drumSteps,
        secondsPerStep
    );
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
    if (TrackProcessor *existing = findTrackProcessor(trackId))
        return existing;

    auto processor =
        std::make_unique<TrackProcessor>(trackId);

    processor->prepare(m_sampleRate, m_blockSize);

    TrackProcessor *result = processor.get();

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

const TrackProcessor* AudioEngine::findTrackProcessor(int trackId) const
{
    for (const auto& processor : m_trackProcessors)
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


void AudioEngine::setOscillatorVolume(
    int trackId,
    float volume)
{
    TrackProcessor *track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setVolume(volume);
}

void AudioEngine::setOscillatorWaveform(
    int trackId,
    Oscillator::Waveform waveform)
{
    TrackProcessor *track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setWaveform(waveform);
}

void AudioEngine::setVelocity(
    int trackId,
    float velocity)
{
    TrackProcessor *track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setVelocity(velocity);
}

void AudioEngine::setAttack(int trackId, float seconds)
{
    TrackProcessor* track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setAttack(seconds);
}

void AudioEngine::setDecay(int trackId, float seconds)
{
    TrackProcessor* track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setDecay(seconds);
}

void AudioEngine::setSustain(int trackId, float level)
{
    TrackProcessor* track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setSustain(level);
}

void AudioEngine::setRelease(int trackId, float seconds)
{
    TrackProcessor* track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setRelease(seconds);
}

void AudioEngine::setPeakGain(int trackId, float level)
{
    TrackProcessor* track =
        findTrackProcessor(trackId);

    if (!track)
        return;

    track->setPeakGain(level);
}