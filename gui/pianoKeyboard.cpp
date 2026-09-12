#include "pianoKeyboard.h"

#include <QPainter>
#include <QMouseEvent>


PianoKeyboard::PianoKeyboard(QWidget *parent)
    : QWidget(parent)
{
    keys = generateKeys();

    setMinimumHeight(150);
    setMouseTracking(true);
}

QVector<PianoKey> PianoKeyboard::generateKeys()
{
    QVector<PianoKey> generatedKeys;

    const QStringList noteNames =
    {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    for (int midi = firstMidiNote; midi <= lastMidiNote; ++midi)
    {
        int noteIndex = midi % 12;
        int octave = (midi / 12) - 1;

        QString name =
            noteNames[noteIndex] + QString::number(octave);

        bool black = noteNames[noteIndex].contains('#');

        generatedKeys.append({
            midi,
            name,
            black
        });
    }

    return generatedKeys;
}

int PianoKeyboard::keyAtPosition(const QPointF &position) const
{
    /*
     * We handle white keys first because black keys sit
     * on top of the white-key areas.
     */

    const int whiteKeyCount = 52;
    const double whiteKeyWidth =
        static_cast<double>(width()) / whiteKeyCount;

    // Check black keys first.
    for (int i = 0; i < keys.size(); ++i)
    {
        if (!keys[i].black)
            continue;

        // Find the white-key index immediately before this black key.
        int whiteIndex = 0;

        for (int j = 0; j < i; ++j)
        {
            if (!keys[j].black)
                ++whiteIndex;
        }

        double blackWidth = whiteKeyWidth * 0.6;
        double blackHeight = height() * 0.6;

        double x =
            (whiteIndex * whiteKeyWidth) - (blackWidth / 2.0);

        QRectF blackRect(
            x,
            0,
            blackWidth,
            blackHeight
        );

        if (blackRect.contains(position))
            return i;
    }

    // If it wasn't a black key, check white keys.
    for (int i = 0; i < keys.size(); ++i)
    {
        if (keys[i].black)
            continue;

        int whiteIndex = 0;

        for (int j = 0; j < i; ++j)
        {
            if (!keys[j].black)
                ++whiteIndex;
        }

        QRectF whiteRect(
            whiteIndex * whiteKeyWidth,
            0,
            whiteKeyWidth,
            height()
        );

        if (whiteRect.contains(position))
            return i;
    }

    return -1;
}

void PianoKeyboard::mousePressEvent(QMouseEvent *event)
{

    if (event->button() != Qt::LeftButton)
        return;

    // Find which key was pressed.
    int midiNote = keyAtPosition(event->position());

    if (midiNote < 0)
        return;

    // Prevent repeated press events while a key is already held.
    if (m_pressedMidiNote != -1)
        return;

    m_pressedMidiNote = midiNote;
    
    emit notePressed(m_pressedMidiNote);

    update();
}

void PianoKeyboard::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (m_pressedMidiNote == -1)
        return;

    emit noteReleased(m_pressedMidiNote);

    m_pressedMidiNote = -1;

    releaseMouse();

    update();
}

void PianoKeyboard::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    const int whiteKeyCount = 52;

    const double whiteKeyWidth =
        static_cast<double>(width()) / whiteKeyCount;

    const double blackKeyWidth =
        whiteKeyWidth * 0.6;

    const double blackKeyHeight =
        height() * 0.6;

    // Background
    painter.fillRect(rect(), QColor("#181818"));

    /*
     * ------------------------------------------------
     * Draw white keys
     * ------------------------------------------------
     */

    int whiteIndex = 0;

    for (const PianoKey &key : keys)
    {
        if (key.black)
            continue;

        QRectF keyRect(
            whiteIndex * whiteKeyWidth,
            0,
            whiteKeyWidth,
            height()
        );

        painter.setBrush(QColor("#f2f2f2"));
        painter.setPen(QPen(QColor("#222222"), 1));

        painter.drawRect(keyRect);

        // Optional key name at the bottom.
        painter.setPen(QColor("#062a42"));

        painter.drawText(
            keyRect.adjusted(0, 0, 0, -8),
            Qt::AlignHCenter | Qt::AlignBottom,
            key.name
        );

        ++whiteIndex;
    }

    /*
     * ------------------------------------------------
     * Draw black keys
     * ------------------------------------------------
     */

    whiteIndex = 0;

    for (int i = 0; i < keys.size(); ++i)
    {
        const PianoKey &key = keys[i];

        if (!key.black)
        {
            ++whiteIndex;
            continue;
        }

        double x =
            (whiteIndex * whiteKeyWidth)
            - (blackKeyWidth / 2.0);

        QRectF keyRect(
            x,
            0,
            blackKeyWidth,
            blackKeyHeight
        );

        painter.setBrush(QColor("#111111"));
        painter.setPen(QPen(QColor("#000000"), 1));

        painter.drawRect(keyRect);
    }
}