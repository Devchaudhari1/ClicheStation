#pragma once

#include <QWidget>
#include <QVector>
#include <QString>

struct PianoKey
{
    int midiNote;
    QString name;
    bool black;
};

class PianoKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit PianoKeyboard(QWidget *parent = nullptr);

signals:
    void notePressed(int midiNote);
    void noteReleased(int midiNote);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QVector<PianoKey> generateKeys();

    int keyAtPosition(const QPointF &position) const;

    QVector<PianoKey> keys;

    int m_pressedMidiNote = -1;

    static constexpr int firstMidiNote = 21; // A0
    static constexpr int lastMidiNote = 108; // C8
};