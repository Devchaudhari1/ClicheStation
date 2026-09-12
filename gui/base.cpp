#include "base.h"
#include "piano.h"
#include "./playlist/Playlist.h"
#include "monitorWindow.h"
#include "drumPad.h"
#include <Qwidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include <QPair>
#include <QSizePolicy>
#include <QResizeEvent>
#include "../audio/AudioEngine.h"
#include "pianoRoll.h"
#include "../midi/MidiInput.h"
#include "voice_track.h"
#include "track_list.h"


void Base::playStartupSound()
{
    m_audioEngine->noteOn(60, 100); // C4

    QTimer::singleShot(180, this, [this]()
    {
        m_audioEngine->noteOff(60);
        m_audioEngine->noteOn(64, 100); // E4
    });

    QTimer::singleShot(360, this, [this]()
    {
        m_audioEngine->noteOff(64);
        m_audioEngine->noteOn(67, 100); // G4
    });

    QTimer::singleShot(540, this, [this]()
    {
        m_audioEngine->noteOff(67);
        m_audioEngine->noteOn(72, 100); // C5
    });

    QTimer::singleShot(720, this, [this]()
    {
        m_audioEngine->noteOff(72);
        m_audioEngine->noteOn(67, 100); // G4
    });

    QTimer::singleShot(900, this, [this]()
    {
        m_audioEngine->noteOff(67);
        m_audioEngine->noteOn(64, 100); // E4
    });

    QTimer::singleShot(1080, this, [this]()
    {
        m_audioEngine->noteOff(64);
        m_audioEngine->noteOn(60, 100); // C4
    });

    QTimer::singleShot(1260, this, [this]()
    {
        m_audioEngine->noteOff(60);
        m_audioEngine->noteOn(72, 100); // C5
    });

    QTimer::singleShot(1440, this, [this]()
    {
        m_audioEngine->noteOff(72);
    });
}


void Base::connectMidi()
{
    MidiInput* input = midiInput();

        connect(
            input,
            &MidiInput::noteOn,
            this,
            [this](int midiNote,
                   int velocity,
                   qint64 timestampNs)
            {
                if (m_activePianoRoll)
                {
                    m_activePianoRoll->midiNoteOn(
                        midiNote,
                        velocity,
                        timestampNs
                    );
                }
            }
        );

        connect(
            input,
            &MidiInput::noteOff,
            this,
            [this](int midiNote,
                   qint64 timestampNs)
            {
                if (m_activePianoRoll)
                {
                    m_activePianoRoll->midiNoteOff(
                        midiNote,
                        timestampNs
                    );
                }
            }
        );

}

Base::Base(QWidget *parent)
    : QMainWindow(parent),
    m_playlist(nullptr)
{
    setWindowTitle("Qt Base");

    m_audioEngine = new AudioEngine(this);
    m_audioEngine->initialize();
    m_audioEngine->start();
    createUI();
    playStartupSound();
    showMaximized();
}

Base::~Base()
{

}

MidiInput* Base::midiInput()
{
    if (!m_midiInput)
    {
        m_midiInput = new MidiInput(this);

        connect(
            m_midiInput,
            &MidiInput::noteOn,
            this,
            [this](int midiNote,
                   int velocity,
                   qint64 timestampNs)
            {

                qDebug() << "Base received Note On:"
                    << midiNote;

                qDebug() << "Active PianoRoll:"
                        << m_activePianoRoll;
                if (!m_activePianoRoll)
                    return;

                m_activePianoRoll->midiNoteOn(
                    midiNote,
                    velocity,
                    timestampNs
                );
            }
        );

        connect(
            m_midiInput,
            &MidiInput::noteOff,
            this,
            [this](int midiNote,
                   qint64 timestampNs)
            {

                if (!m_activePianoRoll)
                    return;

                m_activePianoRoll->midiNoteOff(
                    midiNote,
                    timestampNs
                );
            }
        );
    }

    return m_midiInput;
}
void Base::createUI()
{
    QWidget *window =
        new QWidget(this);

    setCentralWidget(window);

    QVBoxLayout *layout =
        new QVBoxLayout(window);

    layout->setContentsMargins(
        0, 0, 0, 0
    );

    layout->setSpacing(0);

    // -------------------------------------------------
    // Header
    // -------------------------------------------------

    QWidget *Header =
        new QWidget(window);

    Header->setObjectName(
        "Header"
    );

    Header->setFixedHeight(144);

    QHBoxLayout *hLayout =
        new QHBoxLayout(Header);

    hLayout->setContentsMargins(
        18, 10, 18, 10
    );

    hLayout->setSpacing(12);

    // -------------------------------------------------
    // Application title
    // -------------------------------------------------

    QLabel *heading =
        new QLabel("ClicheStation", Header);

    heading->setObjectName("appTitle");

    hLayout->addWidget(
        heading
    );

    // -------------------------------------------------
    // Content Area
    // -------------------------------------------------

    QWidget *contentArea =
        new QWidget(window);

    contentArea->setObjectName(
        "contentArea"
    );

    QVBoxLayout *contentLayout =
        new QVBoxLayout(contentArea);

    contentLayout->setContentsMargins(
        0, 0, 0, 0
    );

    contentLayout->setSpacing(0);

    // -------------------------------------------------
    // Header UI
    // -------------------------------------------------


    QWidget *buttonContainer =
        new QWidget(Header);

    QHBoxLayout *buttonLayout =
        new QHBoxLayout(buttonContainer);

    buttonLayout->setObjectName(
        "buttonLayout"
    );

    buttonContainer->setObjectName(
        "buttonContainer"
    );

    buttonLayout->setContentsMargins(
        8, 6, 8, 6
    );

    buttonLayout->setSpacing(6);

    // -------------------------------------------------
    // Buttons
    // -------------------------------------------------

    QPushButton *ConnectMidiButton =
        new QPushButton("MIDI");

    QPushButton *StartRecording =
        new QPushButton("● Record");

    QPushButton *StopRecording =
        new QPushButton("■ Stop");

    QPushButton *AddSequenceButton =
        new QPushButton("+ Sequence");

    QPushButton *TrackListButton =
        new QPushButton("Tracks");

    QPushButton *PianoButton =
        new QPushButton("Piano");

    QPushButton *DrumPadButton =
        new QPushButton("Drums");


    buttonLayout->addWidget(
        ConnectMidiButton
    );

    buttonLayout->addWidget(
        StartRecording
    );

    buttonLayout->addWidget(
        StopRecording
    );

    buttonLayout->addWidget(
        AddSequenceButton
    );

    buttonLayout->addWidget(
        TrackListButton
    );

    buttonLayout->addWidget(
        PianoButton
    );

    buttonLayout->addWidget(
        DrumPadButton
    );

    QWidget *Spacer =
        new QWidget(Header);

    hLayout->addWidget(
        Spacer,
        2
    );

    hLayout->addWidget(
        buttonContainer
    );

    // -------------------------------------------------
    // Put Header + Content Area into main layout
    // -------------------------------------------------

    layout->addWidget(
        Header
    );

    layout->addWidget(
        contentArea,
        1
    );

    // -------------------------------------------------
    // Playlist
    // -------------------------------------------------

    m_playlist =
        new Playlist(contentArea);

    m_playlist->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    contentLayout->addWidget(
        m_playlist
    );

    // -------------------------------------------------
    // Track List
    // -------------------------------------------------

    m_trackList =
        new TrackList(
            m_audioEngine,
            window
        );

    connect(
        m_trackList,
        &TrackList::pianoRollBecameActive,
        this,
        [this](PianoRoll* pianoRoll)
        {
            qDebug() << "Base: setting active PianoRoll:"
                 << pianoRoll;
            m_activePianoRoll = pianoRoll;
        }
    );
    
    m_trackList->hide();
    
    m_trackList->setGeometry(
    0,
    window->height() - 200,
    window->width(),
    200
);

    m_trackList->setStyleSheet(R"(
        QWidget {
            background-color: #171a1e;
            color: #e6e6e6;
        }

        QPushButton {
            background-color: #252a30;
            color: #dfe3e8;

            border: 1px solid #363c44;
            border-radius: 6px;

            padding: 6px 12px;
        }

        QPushButton:hover {
            background-color: #30363d;
        }

        QPushButton:pressed {
            background-color: #1d2126;
        }
    )");
    // -------------------------------------------------
    // Connections
    // -------------------------------------------------

    connect(
        ConnectMidiButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (!monitorWindow)
                monitorWindow =
                    new MonitorWindow(this);

            monitorWindow->show();
            monitorWindow->raise();
            monitorWindow->activateWindow();
        }
    );

    connect(
        StartRecording,
        &QPushButton::clicked,
        this,
        [this]()
        {
            qDebug() << "START RECORDING BUTTON CLICKED";
            qDebug() << "Active PianoRoll:" << m_activePianoRoll;

            if (!m_activePianoRoll)
            {
                qDebug() << "NO ACTIVE PIANO ROLL";
                return;
            }

            qDebug() << "Calling PianoRoll::startRecording()";

            m_activePianoRoll->startRecording();
        }
    );

    connect(
        StopRecording,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (!m_activePianoRoll)
                return;

            m_activePianoRoll->stopRecording();
        }
    );

    connect(
        AddSequenceButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_playlist->addSequence(
                m_audioEngine,
                m_trackList
            );
        }
    );

    connect(
        m_trackList,
        &TrackList::tracksChanged,
        m_playlist,
        &Playlist::setAvailableTracks
    );

    m_trackList->addVoiceTrack();


    // -------------------------------------------------
    // Styling
    // -------------------------------------------------

    window->setStyleSheet(R"(
        /* -----------------------------------------
        Main application
        ----------------------------------------- */

        QWidget {
            background-color: #101214;
            color: #e6e6e6;
            font-family: "Segoe UI";
            font-size: 13px;
        }


        /* -----------------------------------------
        Header
        ----------------------------------------- */

        QWidget#Header {
            background-color: #181b1f;
            border-bottom: 1px solid #2a2e34;
        }


        QLabel#appTitle {
            color: #f2f2f2;
            font-size: 20px;
            font-weight: 600;
            padding-left: 4px;
        }


        /* -----------------------------------------
        Toolbar
        ----------------------------------------- */

        QWidget#buttonContainer {
            background-color: #20242a;

            border: 1px solid #30353c;
            border-radius: 8px;
        }


        /* -----------------------------------------
        Buttons
        ----------------------------------------- */

        QPushButton {
            background-color: #272c32;
            color: #d8dce1;

            border: 1px solid #383e46;
            border-radius: 6px;

            padding: 7px 12px;

            min-height: 28px;
        }


        QPushButton:hover {
            background-color: #30363e;
            border-color: #4a515a;
        }


        QPushButton:pressed {
            background-color: #1d2126;
        }


        /* -----------------------------------------
        Recording
        ----------------------------------------- */

        QPushButton[text="● Record"] {
            background-color: #3a1f23;
            color: #ff6b6b;
            border-color: #713238;
            font-weight: 600;
        }

        QPushButton[text="● Record"]:hover {
            background-color: #4a252a;
        }


        /* -----------------------------------------
        Stop
        ----------------------------------------- */

        QPushButton[text="■ Stop"] {
            color: #ffb4b4;
        }


        /* -----------------------------------------
        Main content
        ----------------------------------------- */

        QWidget#contentArea {
            background-color: #101214;
        }

        /* =================================================
        Sequence
        ================================================= */

        QWidget#leftSequenceContainer {
            background-color: #15181c;
            border: 1px solid #292e34;
        }

        QWidget#sequenceHeader {
            background-color: #181b1f;
            border: 1px solid #2d3238;
            border-radius: 7px;
        }

        QLabel#sequenceLabel {
            color: #e8eaed;
            font-size: 14px;
            font-weight: 600;
            padding-left: 4px;
        }

        QPushButton#expandButton {
            background-color: #24292f;
            color: #b8bec6;
            border: 1px solid #555d66;
            border-radius: 5px;
            font-size: 12px;
        }

        QPushButton#expandButton:hover {
            background-color: #30363d;
            color: #ffffff;
        }

        QPushButton#expandButton:pressed {
            background-color: #1d2126;
        }

        QPushButton#playButton {
            background-color: #167a52;
            color: #ffffff;
            border: 1px solid #35c79d;
            border-radius: 5px;
            font-size: 14px;
            font-weight: 600;
        }

        QPushButton#playButton:hover {
            background-color: #1b8f60;
        }

        QPushButton#playButton:pressed {
            background-color: #106341;
        }

        QPushButton#addLayerButton {
            background-color: #0757a7;
            color: #ffffff;
            border: 1px solid #4d93d6;
            border-radius: 5px;
            padding: 4px 12px;
        }

        QPushButton#addLayerButton:hover {
            background-color: #176cbd;
        }

        QPushButton#addLayerButton:pressed {
            background-color: #06447f;
        }
    )");

    // -------------------------------------------------
    // Piano
    // -------------------------------------------------

    connect(
        PianoButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (!pianoWindow)
                pianoWindow =
                    new Piano(
                        0,
                        m_audioEngine,
                        nullptr
                    );

            pianoWindow->show();
            pianoWindow->raise();
            pianoWindow->activateWindow();
        }
    );



    // -------------------------------------------------
    // Drum Pad
    // -------------------------------------------------

    connect(
        DrumPadButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (!drumPadWindow)
                drumPadWindow =
                    new DrumPad(nullptr);

            drumPadWindow->show();
            drumPadWindow->raise();
            drumPadWindow->activateWindow();
        }
    );
    // -------------------------------------------------
    // Track List
    // -------------------------------------------------
    
    connect(
        TrackListButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (!m_trackList)
            return;

            m_trackList->setVisible(!m_trackList->isVisible());
            m_trackList->raise();
        }
    );
}

void Base::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (m_trackList)
    {
        m_trackList->setGeometry(
            0,
            centralWidget()->height() - m_trackList->height(),
            centralWidget()->width(),
            m_trackList->height()
        );
    }
}