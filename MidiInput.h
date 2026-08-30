#ifndef MIDIINPUT_H
#define MIDIINPUT_H

#include <QObject>
#include <QStringList>
#include <QByteArray>

#include "RtMidi.h"

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
    void midiMessage(double deltaTime,
                     const QByteArray &data);

    void errorMessage(const QString &message);

private:
    static void midiCallback(
        double deltaTime,
        std::vector<unsigned char> *message,
        void *userData
    );

    RtMidiIn *midiIn;
};

#endif // MIDIINPUT_H
