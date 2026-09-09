#include "pianoRoll.h"

#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include <QDebug>
#include <QCursor>

PianoRoll::PianoRoll(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(800, 500);

    // We need mouseMoveEvent even when no button is pressed.
    setMouseTracking(true);

    setFocusPolicy(Qt::StrongFocus);
}

int PianoRoll::keyFromX(double x) const
{
    double keyWidth =
        static_cast<double>(width()) / keyCount;

    int keyIndex =
        static_cast<int>(x / keyWidth);

    if (keyIndex < 0)
        keyIndex = 0;

    if (keyIndex >= keyCount)
        keyIndex = keyCount - 1;

    return keyIndex;
}

double PianoRoll::timeFromY(double y) const
{
    return y / pixelsPerSecond;
}

int PianoRoll::noteAtPosition(const QPointF &position) const
{
    const double keyWidth =
        static_cast<double>(width()) / keyCount;

    /*
     * Search backwards so that if notes overlap,
     * the most recently created note gets priority.
     */
    for (int i = pressedNotes.size() - 1; i >= 0; --i)
    {
        const PlacedNote &note = pressedNotes[i];

        int keyIndex =
            note.midiNote - firstMidiNote;

        if (keyIndex < 0 || keyIndex >= keyCount)
            continue;

        double x = keyIndex * keyWidth;

        double y =
            note.time * pixelsPerSecond;

        double noteHeight =
            note.duration * pixelsPerSecond;

        QRectF noteRect(
            x + 1,
            y,
            keyWidth - 2,
            noteHeight
        );

        if (noteRect.contains(position))
            return i;
    }

    return -1;
}

bool PianoRoll::isResizeArea(
    const QPointF &position,
    int noteIndex
) const
{
    if (noteIndex < 0 ||
        noteIndex >= pressedNotes.size())
    {
        return false;
    }

    const double keyWidth =
        static_cast<double>(width()) / keyCount;

    const PlacedNote &note =
        pressedNotes[noteIndex];

    int keyIndex =
        note.midiNote - firstMidiNote;

    double x =
        keyIndex * keyWidth;

    double y =
        note.time * pixelsPerSecond;

    double noteHeight =
        note.duration * pixelsPerSecond;

    QRectF noteRect(
        x + 1,
        y,
        keyWidth - 2,
        noteHeight
    );

    /*
     * Bottom 8 pixels are the resize handle.
     */
    QRectF resizeRect(
        noteRect.left(),
        noteRect.bottom() - 8,
        noteRect.width(),
        8
    );

    return resizeRect.contains(position);
}

void PianoRoll::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = event->position();

    /*
     * ----------------------------------------
     * Moving a note
     * ----------------------------------------
     */

    if (draggingNote &&
        selectedNote >= 0 &&
        selectedNote < pressedNotes.size())
    {
        int keyIndex =
            keyFromX(pos.x());

        pressedNotes[selectedNote].midiNote =
            firstMidiNote + keyIndex;

        double newTime =
            (pos.y() - dragOffsetY)
            / pixelsPerSecond;

        if (newTime < 0)
            newTime = 0;

        pressedNotes[selectedNote].time =
            newTime;

        update();
        return;
    }

    /*
     * ----------------------------------------
     * Resizing a note
     * ----------------------------------------
     */

    if (resizingNote &&
        selectedNote >= 0 &&
        selectedNote < pressedNotes.size())
    {
        PlacedNote &note =
            pressedNotes[selectedNote];

        double mouseTime =
            timeFromY(pos.y());

        double newDuration =
            mouseTime - note.time;

        /*
         * Prevent the duration from becoming
         * zero or negative.
         */
        const double minimumDuration = 0.1;

        if (newDuration < minimumDuration)
            newDuration = minimumDuration;

        note.duration =
            newDuration;

        update();
        return;
    }

    /*
     * ----------------------------------------
     * Normal hovering
     * ----------------------------------------
     */

    hoveredKey =
        keyFromX(pos.x());

    cursorY =
        pos.y();

    /*
     * Change cursor when hovering over a
     * resize area.
     */
    int noteIndex =
        noteAtPosition(pos);

    if (noteIndex >= 0 &&
        isResizeArea(pos, noteIndex))
    {
        setCursor(Qt::SizeVerCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }

    update();
}

void PianoRoll::mousePressEvent(QMouseEvent *event)
{
    QPointF pos =
        event->position();

    /*
     * ----------------------------------------
     * RIGHT CLICK = DELETE
     * ----------------------------------------
     */

    if (event->button() == Qt::RightButton)
    {
        int noteIndex =
            noteAtPosition(pos);

        if (noteIndex >= 0)
        {
            qDebug()
                << "Deleted note:"
                << "MIDI ="
                << pressedNotes[noteIndex].midiNote
                << "time ="
                << pressedNotes[noteIndex].time;

            pressedNotes.removeAt(noteIndex);

            selectedNote = -1;

            update();
        }

        return;
    }

    /*
     * ----------------------------------------
     * LEFT CLICK
     * ----------------------------------------
     */

    if (event->button() != Qt::LeftButton)
        return;

    int noteIndex =
        noteAtPosition(pos);
    
    /*
     * ----------------------------------------
     * Clicked an existing note
     * ----------------------------------------
     */

    if (noteIndex >= 0)
    {
        selectedNote =
            noteIndex;

        /*
         * Bottom edge = resize.
         */
        if (isResizeArea(pos, noteIndex))
        {
            resizingNote = true;
            draggingNote = false;

            setCursor(Qt::SizeVerCursor);
        }
        else
        {
            /*
             * Otherwise drag the note.
             */
            draggingNote = true;
            resizingNote = false;

            const PlacedNote &note =
                pressedNotes[noteIndex];

            double noteTop =
                note.time * pixelsPerSecond;

            /*
             * Remember where inside the tile
             * the mouse grabbed it.
             */
            dragOffsetY =
                pos.y() - noteTop;

            setCursor(Qt::SizeAllCursor);
        }

        update();
        return;
    }

    /*
     * ----------------------------------------
     * Clicked empty area = create note
     * ----------------------------------------
     */

   QVector<PlacedNote> before =pressedNotes;

    int keyIndex =
        keyFromX(pos.x());

    int midiNote =
        firstMidiNote + keyIndex;

    double time =
        timeFromY(pos.y());

    PlacedNote newNote;

    newNote.midiNote = midiNote;
    newNote.time = time;
    newNote.duration = defaultDuration;

    pressedNotes.append(newNote);

    saveAction(before);

    selectedNote =
        pressedNotes.size() - 1;

    qDebug()
        << "Created note:"
        << "MIDI =" << midiNote
        << "time =" << time
        << "duration =" << defaultDuration;

    update();
}

void PianoRoll::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    draggingNote = false;
    resizingNote = false;

    setCursor(Qt::ArrowCursor);

    update();
}

void PianoRoll::leaveEvent(QEvent *event)
{
    /*
     * Don't clear the selected note.
     * Just clear the hover information.
     */
    hoveredKey = -1;
    cursorY = -1;

    setCursor(Qt::ArrowCursor);

    update();

    QWidget::leaveEvent(event);
}

void PianoRoll::drawNote(
    QPainter &painter,
    const PlacedNote &note,
    bool selected
)
{
    const double keyWidth =
        static_cast<double>(width()) / keyCount;

    int keyIndex =
        note.midiNote - firstMidiNote;

    if (keyIndex < 0 ||
        keyIndex >= keyCount)
    {
        return;
    }

    double x =
        keyIndex * keyWidth;

    double y =
        note.time * pixelsPerSecond;

    double noteHeight =
        note.duration * pixelsPerSecond;

    /*
     * Keep the note inside the visible area
     * horizontally.
     */
    QRectF noteRect(
        x + 1,
        y,
        keyWidth - 2,
        noteHeight
    );

    /*
     * Selected note gets a brighter border.
     */
    if (selected)
    {
        painter.setBrush(
            QColor("#39d353")
        );

        painter.setPen(
            QPen(QColor("#ffffff"), 2)
        );
    }
    else
    {
        painter.setBrush(
            QColor("#39d353")
        );

        painter.setPen(
            QPen(QColor("#238636"), 1)
        );
    }

    painter.drawRect(noteRect);

    /*
     * Resize handle.
     *
     * This is visually subtle, but gives the
     * user a place to grab the bottom edge.
     */
    if (noteHeight >= 12)
    {
        QRectF handleRect(
            noteRect.left(),
            noteRect.bottom() - 6,
            noteRect.width(),
            6
        );

        painter.fillRect(
            handleRect,
            QColor("#1f8f3a")
        );
    }
}

void PianoRoll::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );

    /*
     * ----------------------------------------
     * Background
     * ----------------------------------------
     */

    painter.fillRect(
        rect(),
        QColor("#181818")
    );

    const double keyWidth =
        static_cast<double>(width()) / keyCount;

    /*
     * ----------------------------------------
     * Vertical piano-key grid
     * ----------------------------------------
     */

    painter.setPen(
        QPen(QColor("#303030"), 1)
    );

    for (int i = 0; i <= keyCount; ++i)
    {
        double x =
            i * keyWidth;

        painter.drawLine(
            QPointF(x, 0),
            QPointF(x, height())
        );
    }

    /*
     * ----------------------------------------
     * Hovered key column
     * ----------------------------------------
     */

    if (hoveredKey >= 0)
    {
        double x =
            hoveredKey * keyWidth;

        QRectF highlightRect(
            x,
            0,
            keyWidth,
            height()
        );

        painter.fillRect(
            highlightRect,
            QColor(80, 180, 100, 60)
        );
    }

    /*
     * ----------------------------------------
     * Horizontal time cursor
     * ----------------------------------------
     */

    if (cursorY >= 0)
    {
        painter.setPen(
            QPen(QColor("#ffffff"), 1)
        );

        painter.drawLine(
            QPointF(0, cursorY),
            QPointF(width(), cursorY)
        );
    }

    /*
     * ----------------------------------------
     * Notes
     * ----------------------------------------
     */

    for (int i = 0; i < pressedNotes.size(); ++i)
    {
        drawNote(
            painter,
            pressedNotes[i],
            i == selectedNote
        );
    }
}

void PianoRoll::copySelectedNote()
{
    if (selectedNote < 0 ||
        selectedNote >= pressedNotes.size())
    {
        return;
    }

    copiedNotes.clear();

    copiedNotes.append(
        pressedNotes[selectedNote]
    );

    qDebug()
        << "Copied note:"
        << "MIDI =" << copiedNotes[0].midiNote
        << "time =" << copiedNotes[0].time
        << "duration =" << copiedNotes[0].duration;
}

void PianoRoll::pasteCopiedNote()
{
    if (copiedNotes.isEmpty())
        return;

    PlacedNote newNote =
        copiedNotes[0];
    QVector<PlacedNote> before = pressedNotes;
    /*
     * Offset the pasted note so it doesn't
     * appear directly on top of the original.
     */
    newNote.midiNote += 1;
    newNote.time += 0.5;

    /*
     * Keep the MIDI note inside A0-C8.
     */
    if (newNote.midiNote > lastMidiNote)
        newNote.midiNote = lastMidiNote;

    /*
     * Don't allow negative time.
     */
    if (newNote.time < 0)
        newNote.time = 0;

    pressedNotes.append(newNote);

    selectedNote =
        pressedNotes.size() - 1;
    saveAction(before);
    qDebug()
        << "Pasted note:"
        << "MIDI =" << newNote.midiNote
        << "time =" << newNote.time
        << "duration =" << newNote.duration;

    update();
}

void PianoRoll::keyPressEvent(QKeyEvent *event)
{
    /*
     * Undo
     */
    if (event->matches(QKeySequence::Undo))
    {
        undo();
        event->accept();
        return;
    }

    /*
     * Redo
     */
    if (event->matches(QKeySequence::Redo))
    {
        redo();
        event->accept();
        return;
    }

    /*
     * Copy
     */
    if (event->matches(QKeySequence::Copy))
    {
        copySelectedNote();
        event->accept();
        return;
    }

    /*
     * Paste
     */
    if (event->matches(QKeySequence::Paste))
    {
        pasteCopiedNote();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}
void PianoRoll::undo()
{
    if (undoStack.isEmpty())
        return;

    PianoAction action =
        undoStack.pop();

    /*
     * Current state becomes the redo state.
     */
    PianoAction redoAction;

    redoAction.before =
        action.before;

    redoAction.after =
        action.after;

    redoStack.push(redoAction);

    /*
     * Restore the previous state.
     */
    pressedNotes =
        action.before;

    selectedNote = -1;

    update();
}

void PianoRoll::redo()
{
    if (redoStack.isEmpty())
        return;

    PianoAction action =
        redoStack.pop();

    /*
     * Put this action back into the undo history.
     */
    undoStack.push(action);

    /*
     * Restore the state after the action.
     */
    pressedNotes =
        action.after;

    selectedNote = -1;

    update();
}

void PianoRoll::saveAction(
    const QVector<PlacedNote> &before
)
{
    PianoAction action;

    action.before = before;
    action.after = pressedNotes;

    undoStack.push(action);

    /*
     * Once a new action is made, the old redo
     * history is no longer valid.
     */
    redoStack.clear();
}
