#ifndef MIDIINPUT_H
#define MIDIINPUT_H

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <chrono>
#include "RtMidi.h"

class Base;

class MidiInput : public QObject
{
    Q_OBJECT

public:
    explicit MidiInput(QObject *parent = nullptr);
    ~MidiInput();

    QStringList availablePorts() const;

    bool openPort(unsigned int portNumber);
    void closePort();

    bool isOpen() const;

signals:
    void midiMessage(
        double deltaTime,
        const QByteArray &data
    );

    void noteOn(
        int midiNote,
        int velocity,
        qint64 timestampNs
    );

    void noteOff(
        int midiNote,
        qint64 timestampNs
    );

    void errorMessage(
        const QString &message
    );

private:
    static void midiCallback(
        double deltaTime,
        std::vector<unsigned char> *message,
        void *userData
    );
    Base* m_base = nullptr;
    RtMidiIn *midiIn;
};

#endif // MIDIINPUT_H
