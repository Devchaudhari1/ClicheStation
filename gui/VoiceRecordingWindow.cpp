#include "VoiceRecordingWindow.h"

#include "../audio/AudioEngine.h"
#include "./playlist/Clip.h"
#include "RecordingTimeRuler.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QDebug>

#include <algorithm>
#include <cmath>
#include <cstddef>

class RecordingTimeRuler;

VoiceRecordingWindow::VoiceRecordingWindow(
    AudioEngine* audioEngine,
    QWidget* parent
)
    : QWidget(parent)
    , m_audioEngine(audioEngine)
{
    setWindowFlags(
        Qt::Window |
        Qt::WindowTitleHint |
        Qt::WindowSystemMenuHint |
        Qt::WindowMinimizeButtonHint |
        Qt::WindowMaximizeButtonHint |
        Qt::WindowCloseButtonHint
    );
    setWindowTitle("Voice Recording");
    resize(900, 300);

    createUI();
    loadRecording();
}

void VoiceRecordingWindow::createUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );
    scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_timelineWidget = new QWidget;
    auto* timelineLayout = new QVBoxLayout(m_timelineWidget);

    timelineLayout->setContentsMargins(0, 0, 0, 0);
    timelineLayout->setSpacing(5);

    m_timeRuler = new RecordingTimeRuler;

    m_timeRuler->setStyleSheet(
        "RecordingTimeRuler {"
        "    background-color: #2a2a2a;"
        "}"
    );

    timelineLayout->addWidget(m_timeRuler);

    m_clip = new Clip(
        0,
        "Voice Recording",
        0.0,
        0.05,
        m_timelineWidget
    );

    timelineLayout->addWidget(m_clip);

    scrollArea->setWidget(m_timelineWidget);

    mainLayout->addWidget(
        scrollArea,
        1
    );

    auto* buttonLayout = new QHBoxLayout;

    m_playButton = new QPushButton("Play", this);
    m_stopButton = new QPushButton("Stop", this);
    m_closeButton = new QPushButton("Close", this);

    buttonLayout->addWidget(m_playButton);
    buttonLayout->addWidget(m_stopButton);

    buttonLayout->addStretch();

    buttonLayout->addWidget(m_closeButton);

    mainLayout->addLayout(buttonLayout);

    setStyleSheet(
        "VoiceRecordingWindow {"
        "    background-color: #202020;"
        "}"
    );
    connect(
        m_closeButton,
        &QPushButton::clicked,
        this,
        &QWidget::close
    );

    connect(
        m_timeRuler,
        &RecordingTimeRuler::positionClicked,
        this,
        [](double time)
        {
            qDebug()
                << "Recording playback position:"
                << time
                << "seconds";
        }
    );
    connect(
        m_playButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (!m_audioEngine)
                return;

            m_audioEngine->startRecordingPlayback(0);
        }
    );

    connect(
        m_stopButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (!m_audioEngine)
                return;

            m_audioEngine->stopRecordingPlayback();
        }
    );
}

void VoiceRecordingWindow::loadRecording()
{
    if (!m_audioEngine || !m_clip)
        return;

    const auto& left =
        m_audioEngine->recordedLeft();

    const auto& right =
        m_audioEngine->recordedRight();

    const std::size_t sampleCount =
        m_audioEngine->recordedSamples();

    const double sampleRate =
        m_audioEngine->sampleRate();

    if (sampleCount == 0 || sampleRate <= 0.0)
        return;

    m_monoSamples.resize(
        static_cast<qsizetype>(sampleCount)
    );

    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        const float leftSample =
            i < left.size() ? left[i] : 0.0f;

        const float rightSample =
            i < right.size() ? right[i] : 0.0f;

        m_monoSamples[static_cast<qsizetype>(i)] =
            0.5f * (leftSample + rightSample);
    }


    const double duration =
        static_cast<double>(sampleCount) / sampleRate;

    m_timeRuler->setDuration(duration);
    m_clip->setDuration(duration);
    m_clip->setSourceRange(0.0, duration);

    constexpr double PixelsPerSecond = 100.0;

    const int waveformWidth =
        std::max(
            1,
            static_cast<int>(
                duration * PixelsPerSecond
            )
        );

    m_timelineWidget->setFixedWidth(waveformWidth);

    m_clip->setSamples(
        m_monoSamples,
        waveformWidth
    );
}