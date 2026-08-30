#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QCheckBox>

#include "MidiInput.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshPorts();
    void connectMidi();

    void processMidiMessage(
        double deltaTime,
        const QByteArray &data
    );

    void showError(const QString &message);

private:
    QString noteName(int note) const;
    QString decodeMidiMessage(
        double deltaTime,
        const QByteArray &data
    );

    MidiInput *midiInput;

    QComboBox *portCombo;
    QPushButton *refreshButton;
    QPushButton *connectButton;

    QTableWidget *messageTable;

    QLabel *statusLabel;

    bool connected = false;
};

#endif
