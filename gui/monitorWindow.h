#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QMainWindow>

class QComboBox;
class QPushButton;
class QLabel;
class QTableWidget;
class QString;
class QByteArray;

class MidiInput;

class MonitorWindow : public QMainWindow
{
public:
    explicit MonitorWindow(QWidget *parent = nullptr);
    ~MonitorWindow();

private:
    void refreshPorts();
    void connectMidi();

    void processMidiMessage(
        double deltaTime,
        const QByteArray &data
    );

    QString noteName(int note) const;

    QString decodeMidiMessage(
        double deltaTime,
        const QByteArray &data
    );

    void showError(const QString &message);

private:
    QComboBox *portCombo;
    QPushButton *refreshButton;
    QPushButton *connectButton;
    QLabel *statusLabel;
    QTableWidget *messageTable;

    MidiInput *midiInput;

    bool connected = false;
};

#endif // MONITORWINDOW_H