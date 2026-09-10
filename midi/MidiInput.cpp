#include "MidiInput.h"

MidiInput::MidiInput(QObject *parent)
    : QObject(parent),
      midiIn(nullptr)
{
    try
    {
        midiIn = new RtMidiIn();
    }
    catch (RtMidiError &error)
    {
        emit errorMessage(
            QString("Could not create MIDI input: %1")
                .arg(QString::fromStdString(error.getMessage()))
        );
    }
}

MidiInput::~MidiInput()
{
    closePort();

    delete midiIn;
}

QStringList MidiInput::availablePorts() const
{
    QStringList ports;

    if (!midiIn)
        return ports;

    try
    {
        unsigned int count = midiIn->getPortCount();

        for (unsigned int i = 0; i < count; ++i)
        {
            std::string name = midiIn->getPortName(i);

            ports.append(
                QString::fromStdString(name)
            );
        }
    }
    catch (RtMidiError &error)
    {
        // Could emit an error here if required.
    }

    return ports;
}

bool MidiInput::openPort(unsigned int portNumber)
{
    if (!midiIn)
        return false;

    try
    {
        if (midiIn->isPortOpen())
            midiIn->closePort();

        // Ignore MIDI System Exclusive,
        // MIDI timing messages and Active Sensing.
        midiIn->ignoreTypes(
            true,   // sysex
            true,   // timing
            true    // active sensing
        );

        midiIn->openPort(portNumber);

        /*
         * Important:
         * Set the callback immediately after opening
         * the port.
         */
        midiIn->setCallback(
            &MidiInput::midiCallback,
            this
        );

        return true;
    }
    catch (RtMidiError &error)
    {
        emit errorMessage(
            QString::fromStdString(error.getMessage())
        );

        return false;
    }
}

void MidiInput::closePort()
{
    if (!midiIn)
        return;

    try
    {
        if (midiIn->isPortOpen())
        {
            midiIn->cancelCallback();
            midiIn->closePort();
        }
    }
    catch (...)
    {
        // Ignore errors during shutdown.
    }
}

bool MidiInput::isOpen() const
{
    return midiIn && midiIn->isPortOpen();
}

void MidiInput::midiCallback(
    double deltaTime,
    std::vector<unsigned char> *message,
    void *userData)
{
    if (!userData || !message)
        return;

    MidiInput *self =
        static_cast<MidiInput *>(userData);

    if (message->size() < 3)
        return;

    const unsigned char status =
        (*message)[0];

    const unsigned char data1 =
        (*message)[1];

    const unsigned char data2 =
        (*message)[2];

    const unsigned char messageType =
        status & 0xF0;

    const int midiNote =
        static_cast<int>(data1);

    const int velocity =
        static_cast<int>(data2);

    const auto now =
        std::chrono::steady_clock::now();

    const qint64 timestampNs =
        std::chrono::duration_cast<
            std::chrono::nanoseconds
        >(
            now.time_since_epoch()
        ).count();

    // --------------------------------------------
    // Note On
    // --------------------------------------------

    if (messageType == 0x90)
    {
        if (velocity > 0)
        {
            emit self->noteOn(
                midiNote,
                velocity,
                timestampNs
            );
        }
        else
        {
            // MIDI convention:
            // Note On with velocity 0 = Note Off

            emit self->noteOff(
                midiNote,
                timestampNs
            );
        }
    }

    // --------------------------------------------
    // Note Off
    // --------------------------------------------

    else if (messageType == 0x80)
    {
        emit self->noteOff(
            midiNote,
            timestampNs
        );
    }

    // --------------------------------------------
    // Existing generic MIDI signal
    // --------------------------------------------

    QByteArray data;

    for (unsigned char byte : *message)
    {
        data.append(
            static_cast<char>(byte)
        );
    }

    emit self->midiMessage(
        deltaTime,
        data
    );
}