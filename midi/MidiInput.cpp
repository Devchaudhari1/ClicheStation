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

    QByteArray data;

    for (unsigned char byte : *message)
    {
        data.append(
            static_cast<char>(byte)
        );
    }

    /*
     * Qt's signal/slot mechanism will safely transfer
     * the event to the GUI thread when appropriate.
     */
    emit self->midiMessage(
        deltaTime,
        data
    );
}
