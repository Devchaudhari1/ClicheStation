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

    QLabel *heading =
        new QLabel("Heading", Header);

    QHBoxLayout *hLayout =
        new QHBoxLayout(Header);

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

    buttonContainer->setFixedHeight(40);

    QPushButton *AddSequenceButton =
        new QPushButton("+ Add Sequence");

    QPushButton *StartRecording =
        new QPushButton("Start Recording");

    QPushButton *StopRecording =
        new QPushButton("Stop Recording");

    QPushButton *PianoButton =
        new QPushButton("Piano");

    QPushButton *DrumPadButton =
        new QPushButton("Drum Pad");

    QPushButton *TrackListButton =
        new QPushButton("Track List");

    QPushButton *ConnectMidiButton=
        new QPushButton("Connect Midi");

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
            background-color: #171717;
            color: white;
        }

        QPushButton {
            background-color: #2b2b2b;
            color: white;
            border: 1px solid #444444;
            border-radius: 6px;
            padding: 6px 12px;
        }

        QPushButton:hover {
            background-color: #3a3a3a;
        }

        QPushButton:pressed {
            background-color: #222222;
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
            if (!m_activePianoRoll)
                return;

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
        QWidget#Header {
            background-color: #230c0c;
            border-color: #918c8c;
            color: white;
        }

        QWidget#buttonContainer {
            background-color: #297731;
            border-radius: 20px;
            border-color: #918c8c;
            border: 1px solid;
            color: white;
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