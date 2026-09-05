#include "base.h"
#include "piano.h"
#include "monitorWindow.h"
#include "drumPad.h"
#include <Qwidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include "../audio/AudioEngine.h"
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


Base::Base(QWidget *parent)
    : QMainWindow(parent)
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



void Base::createUI()
{
    QWidget * window = new QWidget(this);
    setCentralWidget(window);
    QVBoxLayout * layout = new QVBoxLayout(window);
    // QWidget * HorizontalBar= new QWidget(window);
    // QWidget * VerticalBar= new QWidget(window);
    QWidget * Header= new QWidget(window);
    Header->setObjectName("Header");
    QWidget * Sidebar= new QWidget(window);
    QLabel * heading = new QLabel("Heading", Header);
    QHBoxLayout * hLayout = new QHBoxLayout(Header);
    QWidget * buttonContainer= new QWidget(Header);
    QHBoxLayout * buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setObjectName("buttonLayout");
    buttonContainer->setObjectName("buttonContainer");
    buttonContainer->setFixedHeight(40);
    QPushButton * StartRecording = new QPushButton("Start Recording");
    QPushButton * StopRecording = new QPushButton("Stop Recording");
    QPushButton * PianoButton = new QPushButton("Piano");
    QPushButton * MonitorButton = new QPushButton("Monitor");
    QPushButton * DrumPadButton = new QPushButton("Drum Pad");
    buttonLayout->addWidget(StartRecording);
    buttonLayout->addWidget(StopRecording); 
    buttonLayout->addWidget(PianoButton);
    buttonLayout->addWidget(MonitorButton);
    buttonLayout->addWidget(DrumPadButton);
    QWidget * Spacer= new QWidget(Header);
    hLayout->addWidget(Spacer,2);
    hLayout->addWidget(buttonContainer);

    QWidget * ContentArea= new QWidget(window);
    ContentArea->setObjectName("ContentArea");
    
    layout->addWidget(Header);
    layout->addWidget(ContentArea, 1);
    TrackList *trackList = new TrackList(m_audioEngine, window);
    trackList->addVoiceTrack();

    layout->addWidget(trackList);
    QWidget * Footer= new QWidget(window);
    Footer->setObjectName("Footer");
    layout->addWidget(Footer,2);
    window->setStyleSheet(R"(
        QWidget#Header {
            background-color: #230c0c;
            border-color: #918c8c;
            color: white;
        }
        QWidget#ContentArea {
            background-color: #1c1c1c;
            border-color: #918c8c;
            color: white;
        }
        QWidget#buttonContainer {
            background-color: #297731;
            border-radius:20px;
            border-color: #918c8c;
            border:1px solid;
            color: white;
        }
        QWidget#Footer {
            background-color: #230c0c;
            border-color: #918c8c;
            color: white;
        }
    )");
    connect(PianoButton, &QPushButton::clicked, this, [this]()
    {
        if(!pianoWindow)
            pianoWindow = new Piano(m_audioEngine, nullptr);
        pianoWindow->show();
        pianoWindow->raise();
        pianoWindow->activateWindow();
    });
    connect(MonitorButton, &QPushButton::clicked, this, [this]()
    {
        if(!monitorWindow)
            monitorWindow = new MonitorWindow(nullptr);
        monitorWindow->show();
        monitorWindow->raise();
        monitorWindow->activateWindow();
    });
    connect(DrumPadButton, &QPushButton::clicked, this, [this]()
    {
        if(!drumPadWindow)
            drumPadWindow = new DrumPad(nullptr);
        drumPadWindow->show();
        drumPadWindow->raise();
        drumPadWindow->activateWindow();
    });
}