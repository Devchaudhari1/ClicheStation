#include "monitorWindow.h"
#include "../midi/MidiInput.h"
#include "base.h"
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>

MonitorWindow::MonitorWindow(Base* base, QWidget* parent)
    : QMainWindow(parent),
      m_base(base),
      m_midiInput(nullptr)
{
    setWindowTitle("MIDI Monitor");

    resize(900, 600);

    // --------------------------------------------------
    // Top controls
    // --------------------------------------------------

    portCombo = new QComboBox(this);
    portCombo->setObjectName("midiPortCombo");

    refreshButton =
        new QPushButton("Refresh", this);
    refreshButton->setObjectName("midiRefreshButton");

    connectButton =
        new QPushButton("Connect", this);
    connectButton->setObjectName("midiConnectButton");

    statusLabel =
        new QLabel("Disconnected", this);
    statusLabel->setObjectName("midiStatusLabel");

    // --------------------------------------------------
    // MIDI message table
    // --------------------------------------------------

    messageTable =
        new QTableWidget(this);
    messageTable->setObjectName("midiMessageTable");

    messageTable->setColumnCount(6);

    messageTable->setHorizontalHeaderLabels({
        "Time",
        "Type",
        "Channel",
        "Note / Data",
        "Velocity / Value",
        "Raw MIDI"
    });

    messageTable->horizontalHeader()
        ->setStretchLastSection(true);

    messageTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    messageTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );


    // --------------------------------------------------
    // Layout
    // --------------------------------------------------

    auto *topLayout =
        new QHBoxLayout();

    topLayout->addWidget(
        new QLabel("MIDI Input:")
    );

    topLayout->addWidget(
        portCombo
    );

    topLayout->addWidget(
        refreshButton
    );

    topLayout->addWidget(
        connectButton
    );

    topLayout->addWidget(
        statusLabel
    );

    auto *topBar = new QWidget(this);
    topBar->setObjectName("midiTopBar");

    topBar->setLayout(topLayout);

    auto *MonitorLayout =
        new QVBoxLayout();

    MonitorLayout->addWidget(
        topBar
    );

    MonitorLayout->addWidget(
        messageTable
    );


    auto *central =
        new QWidget(this);
    central->setObjectName("midiMonitor");
    central->setLayout(MonitorLayout);

    setCentralWidget(central);
    //Handle null cases
    if (!m_base)
    {
        statusLabel->setText("No Base connection");
        return;
    }

    m_midiInput = m_base->midiInput();

    if (!m_midiInput)
    {
        statusLabel->setText("MIDI input unavailable");
        return;
    }
    // --------------------------------------------------
    // Connections
    // --------------------------------------------------

    connect(
        refreshButton,
        &QPushButton::clicked,
        this,
        &MonitorWindow::refreshPorts
    );

    connect(
        connectButton,
        &QPushButton::clicked,
        this,
        &MonitorWindow::connectMidi
    );

    connect(
        m_midiInput,
        &MidiInput::midiMessage,
        this,
        &MonitorWindow::processMidiMessage
    );

    connect(
        m_midiInput,
        &MidiInput::errorMessage,
        this,
        &MonitorWindow::showError
    );


    // Initial port scan
    refreshPorts();

    //Stylesheets

    setStyleSheet(R"(
        /* --------------------------------------------------
        Main window
        -------------------------------------------------- */

        QMainWindow {
            background-color: #101214;
            color: #e6e9ed;
        }

        QWidget#midiMonitor {
            background-color: #101214;
        }


        /* --------------------------------------------------
        Top control bar
        -------------------------------------------------- */

        QWidget#midiTopBar {
            background-color: #181b1f;
            border: 1px solid #2b3036;
            border-radius: 8px;
        }


        /* --------------------------------------------------
        Labels
        -------------------------------------------------- */

        QWidget#midiTopBar QLabel {
            color: #aeb5bd;
            font-size: 13px;
        }

        QLabel#midiStatusLabel {
            color: #d0d5da;
            font-weight: 600;
            padding-left: 8px;
        }


        /* --------------------------------------------------
        MIDI port combo
        -------------------------------------------------- */

        QComboBox#midiPortCombo {
            background-color: #20252b;
            color: #e6e9ed;
            border: 1px solid #383e46;
            border-radius: 6px;
            padding: 3px 8px;
        }

        QComboBox#midiPortCombo:hover {
            border-color: #4b535d;
        }

        QComboBox#midiPortCombo:focus {
            border-color: #3978b8;
        }

        QComboBox#midiPortCombo::drop-down {
            border: none;
            width: 24px;
        }

        QComboBox#midiPortCombo QAbstractItemView {
            background-color: #20252b;
            color: #e6e9ed;
            border: 1px solid #383e46;
            selection-background-color: #315f8f;
            selection-color: white;
        }


        /* --------------------------------------------------
        Buttons
        -------------------------------------------------- */

        QPushButton#midiRefreshButton,
        QPushButton#midiConnectButton {
            background-color: #242a31;
            color: #dce1e6;
            border: 1px solid #3a424b;
            border-radius: 6px;
            padding: 4px 14px;
            font-weight: 600;
        }

        QPushButton#midiRefreshButton:hover,
        QPushButton#midiConnectButton:hover {
            background-color: #2d353e;
            border-color: #4c5661;
        }

        QPushButton#midiRefreshButton:pressed,
        QPushButton#midiConnectButton:pressed {
            background-color: #1c2126;
        }

        QPushButton#midiConnectButton {
            background-color: #155f91;
            border-color: #267ab2;
            color: #f2f7fb;
        }

        QPushButton#midiConnectButton:hover {
            background-color: #1d70a6;
            border-color: #358bc1;
        }

        QPushButton#midiConnectButton:pressed {
            background-color: #104b72;
        }


        /* --------------------------------------------------
        MIDI table
        -------------------------------------------------- */

        QTableWidget#midiMessageTable {
            background-color: #15181c;
            alternate-background-color: #191d22;
            color: #dce1e6;

            border: 1px solid #2b3036;
            border-radius: 8px;

            gridline-color: #292e34;

            selection-background-color: #294e73;
            selection-color: #ffffff;
        }

        QTableWidget#midiMessageTable::item {
            padding: 5px;
            border: none;
        }

        QTableWidget#midiMessageTable::item:selected {
            background-color: #294e73;
            color: #ffffff;
        }


        /* --------------------------------------------------
        Table header
        -------------------------------------------------- */

        QHeaderView::section {
            background-color: #20252b;
            color: #aeb6bf;

            border: none;
            border-right: 1px solid #30363d;
            border-bottom: 1px solid #383e46;

            padding: 6px 8px;

            font-weight: 600;
        }

        QHeaderView::section:last {
            border-right: none;
        }


        /* --------------------------------------------------
        Scrollbars
        -------------------------------------------------- */

        QScrollBar:vertical {
            background: #121518;
            width: 10px;
            margin: 2px;
        }

        QScrollBar::handle:vertical {
            background: #3a4149;
            border-radius: 5px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background: #505963;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background: #121518;
            height: 10px;
            margin: 2px;
        }

        QScrollBar::handle:horizontal {
            background: #3a4149;
            border-radius: 5px;
            min-width: 30px;
        }

        QScrollBar::handle:horizontal:hover {
            background: #505963;
        }

        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {
            width: 0px;
        }
    )");
}

MonitorWindow::~MonitorWindow()
{
}

// ------------------------------------------------------
// Find MIDI devices
// ------------------------------------------------------

void MonitorWindow::refreshPorts()
{
    portCombo->clear();

    QStringList ports =
        m_midiInput->availablePorts();

    portCombo->addItems(ports);

    if (ports.isEmpty())
    {
        statusLabel->setText(
            "No MIDI devices found"
        );
    }
    else
    {
        statusLabel->setText(
            QString("%1 MIDI device(s)")
                .arg(ports.size())
        );
    }
}


// ------------------------------------------------------
// Connect to selected MIDI device
// ------------------------------------------------------

void MonitorWindow::connectMidi()
{
    if (portCombo->count() == 0)
        return;

        MidiInput* m_midiInput = m_base->midiInput();

    if (!connected)
    {
        int port =
            portCombo->currentIndex();

        if (m_midiInput->openPort(port))
        {
            connected = true;

            connectButton->setText(
                "Disconnect"
            );

            portCombo->setEnabled(false);

            statusLabel->setText(
                "Connected"
            );
        }
    }
    else
    {
        m_midiInput->closePort();

        connected = false;

        connectButton->setText(
            "Connect"
        );

        portCombo->setEnabled(true);

        statusLabel->setText(
            "Disconnected"
        );
    }
}


// ------------------------------------------------------
// MIDI message received
// ------------------------------------------------------

void MonitorWindow::processMidiMessage(
    double deltaTime,
    const QByteArray &data)
{
    if (data.isEmpty())
        return;

    QString decoded =
        decodeMidiMessage(
            deltaTime,
            data
        );

    qDebug() << decoded;

    /*
     * We will split the information into columns below.
     */

    const unsigned char status =
        static_cast<unsigned char>(
            data[0]
        );

    const int messageType =
        status & 0xF0;

    const int channel =
        (status & 0x0F) + 1;


    QString type;
    QString noteData;
    QString velocityValue;


    // ----------------------------------------------
    // Note On
    // ----------------------------------------------

    if (messageType == 0x90 &&
        data.size() >= 3)
    {
        int note =
            static_cast<unsigned char>(
                data[1]
            );

        int velocity =
            static_cast<unsigned char>(
                data[2]
            );

        /*
         * MIDI convention:
         *
         * Note On with velocity 0
         * is equivalent to Note Off.
         */

        if (velocity == 0)
        {
            type = "Note Off";
        }
        else
        {
            type = "Note On";
        }

        noteData =
            QString("%1 (%2)")
                .arg(noteName(note))
                .arg(note);

        velocityValue =
            QString::number(velocity);
    }


    // ----------------------------------------------
    // Note Off
    // ----------------------------------------------

    else if (messageType == 0x80 &&
             data.size() >= 3)
    {
        int note =
            static_cast<unsigned char>(
                data[1]
            );

        int velocity =
            static_cast<unsigned char>(
                data[2]
            );

        type = "Note Off";

        noteData =
            QString("%1 (%2)")
                .arg(noteName(note))
                .arg(note);

        velocityValue =
            QString::number(velocity);
    }


    // ----------------------------------------------
    // Control Change
    // ----------------------------------------------

    else if (messageType == 0xB0 &&
             data.size() >= 3)
    {
        int controller =
            static_cast<unsigned char>(
                data[1]
            );

        int value =
            static_cast<unsigned char>(
                data[2]
            );

        type = "Control Change";

        noteData =
            QString("CC %1")
                .arg(controller);

        velocityValue =
            QString::number(value);
    }


    // ----------------------------------------------
    // Program Change
    // ----------------------------------------------

    else if (messageType == 0xC0 &&
             data.size() >= 2)
    {
        int program =
            static_cast<unsigned char>(
                data[1]
            );

        type = "Program Change";

        noteData =
            QString::number(program);

        velocityValue = "-";
    }


    // ----------------------------------------------
    // Channel Aftertouch
    // ----------------------------------------------

    else if (messageType == 0xD0 &&
             data.size() >= 2)
    {
        int pressure =
            static_cast<unsigned char>(
                data[1]
            );

        type = "Channel Aftertouch";

        noteData = "-";

        velocityValue =
            QString::number(pressure);
    }


    // ----------------------------------------------
    // Pitch Bend
    // ----------------------------------------------

    else if (messageType == 0xE0 &&
             data.size() >= 3)
    {
        int lsb =
            static_cast<unsigned char>(
                data[1]
            );

        int msb =
            static_cast<unsigned char>(
                data[2]
            );

        int value =
            lsb | (msb << 7);

        /*
         * MIDI pitch bend is 14-bit:
         *
         * 0     = minimum
         * 8192  = center
         * 16383 = maximum
         */

        int signedValue =
            value - 8192;

        type = "Pitch Bend";

        noteData =
            QString::number(
                signedValue
            );

        velocityValue =
            QString::number(value);
    }


    // ----------------------------------------------
    // Polyphonic Aftertouch
    // ----------------------------------------------

    else if (messageType == 0xA0 &&
             data.size() >= 3)
    {
        int note =
            static_cast<unsigned char>(
                data[1]
            );

        int pressure =
            static_cast<unsigned char>(
                data[2]
            );

        type = "Poly Aftertouch";

        noteData =
            QString("%1 (%2)")
                .arg(noteName(note))
                .arg(note);

        velocityValue =
            QString::number(
                pressure
            );
    }


    // ----------------------------------------------
    // Other MIDI messages
    // ----------------------------------------------

    else
    {
        type = "Other";

        noteData = "-";
        velocityValue = "-";
    }


    // ----------------------------------------------
    // Raw bytes
    // ----------------------------------------------

    QString raw;

    for (unsigned char byte : data)
    {
        raw += QString("%1 ")
                   .arg(
                       byte,
                       2,
                       16,
                       QChar('0')
                   )
                   .toUpper();
    }

    // ----------------------------------------------
    // Add table row
    // ----------------------------------------------

    int row =
        messageTable->rowCount();

    messageTable->insertRow(row);

    messageTable->setItem(
        row,
        0,
        new QTableWidgetItem(
            QDateTime::currentDateTime()
                .toString("HH:mm:ss.zzz")
        )
    );

    messageTable->setItem(
        row,
        1,
        new QTableWidgetItem(type)
    );

    messageTable->setItem(
        row,
        2,
        new QTableWidgetItem(
            QString::number(channel)
        )
    );

    messageTable->setItem(
        row,
        3,
        new QTableWidgetItem(noteData)
    );

    messageTable->setItem(
        row,
        4,
        new QTableWidgetItem(
            velocityValue
        )
    );

    messageTable->setItem(
        row,
        5,
        new QTableWidgetItem(raw)
    );

    messageTable->scrollToBottom();


    /*
     * Prevent the table from growing forever.
     *
     * This is useful when testing keyboards that
     * produce lots of MIDI events.
     */
    constexpr int maxRows = 1000;

    while (messageTable->rowCount() > maxRows)
    {
        messageTable->removeRow(0);
    }
}


// ------------------------------------------------------
// Convert MIDI note number to musical note
// ------------------------------------------------------

QString MonitorWindow::noteName(int note) const
{
    static const QString names[] =
    {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    if (note < 0 || note > 127)
        return "?";

    int octave =
        (note / 12) - 1;

    return QString("%1%2")
        .arg(names[note % 12])
        .arg(octave);
}


// ------------------------------------------------------
// Debug decoder
// ------------------------------------------------------

QString MonitorWindow::decodeMidiMessage(
    double deltaTime,
    const QByteArray &data)
{
    if (data.isEmpty())
        return {};

    const unsigned char status =
        static_cast<unsigned char>(
            data[0]
        );

    const int type =
        status & 0xF0;

    const int channel =
        (status & 0x0F) + 1;

    QString result;

    result +=
        QString("dt=%1 ms ")
            .arg(deltaTime * 1000.0, 0, 'f', 3);

    result +=
        QString("channel=%1 ")
            .arg(channel);


    if (type == 0x90 && data.size() >= 3)
    {
        int note =
            static_cast<unsigned char>(
                data[1]
            );

        int velocity =
            static_cast<unsigned char>(
                data[2]
            );

        result +=
            QString("NOTE_ON %1 (%2) velocity=%3")
                .arg(noteName(note))
                .arg(note)
                .arg(velocity);
    }

    else if (type == 0x80 && data.size() >= 3)
    {
        int note =
            static_cast<unsigned char>(
                data[1]
            );

        int velocity =
            static_cast<unsigned char>(
                data[2]
            );

        result +=
            QString("NOTE_OFF %1 (%2) velocity=%3")
                .arg(noteName(note))
                .arg(note)
                .arg(velocity);
    }

    else
    {
        result += "MIDI";

        for (unsigned char byte : data)
        {
            result += QString(" %1")
                .arg(byte, 2, 16, QChar('0'))
                .toUpper();
        }
    }

    return result;
}


// ------------------------------------------------------
// Error
// ------------------------------------------------------

void MonitorWindow::showError(
    const QString &message)
{       
    QMessageBox::warning(
        this,
        "MIDI Error",
        message
    );

    statusLabel->setText(
        "MIDI error"
    );
}
