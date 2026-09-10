#include "pianoRoll.h"

#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include <QDebug>
#include <QCursor>
#include <QFrame>

PianoRoll::PianoRoll(QWidget *parent)
    : QWidget(parent),
      m_contentHeight(500.0),
      m_originY(500.0),
      m_recordingLine(nullptr)
{
    setMinimumSize(800, 500);

    setMouseTracking(true);

    setFocusPolicy(Qt::StrongFocus);

    m_recordingLine = new QFrame(this);

    m_recordingLine->setFixedHeight(1);
    m_recordingLine->setStyleSheet(
        "background-color: red;"
    );

    m_recordingLine->hide();
}


const QVector<PlacedNote>& PianoRoll::notes() const
{
    return pressedNotes;
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

double PianoRoll::yFromTime(double time) const
{
    return m_originY - time * pixelsPerSecond;
}

double PianoRoll::timeFromY(double y) const
{
    return (m_originY - y) / pixelsPerSecond;
}

void PianoRoll::ensureContentHeight(double time)
{
    const double requiredHeight =
        m_originY +
        time * pixelsPerSecond;

    if (requiredHeight <= m_contentHeight)
        return;

    m_contentHeight = requiredHeight;

    setMinimumHeight(
        static_cast<int>(m_contentHeight)
    );

    resize(
        width(),
        static_cast<int>(m_contentHeight)
    );
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
            yFromTime(note.time);

        double noteHeight =
            note.duration * pixelsPerSecond;

        y -= noteHeight;

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
        yFromTime(note.time);

    double noteHeight =
        note.duration * pixelsPerSecond;

    y -= noteHeight;

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
            timeFromY(pos.y() - dragOffsetY);

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

void PianoRoll::focusInEvent(QFocusEvent* event)
{
    QWidget::focusInEvent(event);
    emit becameActive(this);
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

            // double noteTop =
            //     note.time * pixelsPerSecond;

            /*
             * Remember where inside the tile
             * the mouse grabbed it.
             */
            double noteTop =
                yFromTime(note.time) -
                note.duration * pixelsPerSecond;

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

    const int keyIndex =
        note.midiNote - firstMidiNote;

    if (keyIndex < 0 || keyIndex >= keyCount)
        return;

    const double x =
        keyIndex * keyWidth;

    const double noteHeight =
        note.duration * pixelsPerSecond;

    const double bottomY =
        yFromTime(note.time);

    const double topY =
        bottomY - noteHeight;

    QRectF noteRect(
        x + 1,
        topY,
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

     // Recording
    if (m_recording &&
        recordingCursorY >= 0)
    {
        painter.setPen(
            QPen(QColor("#ff4040"), 2)
        );

        painter.drawLine(
            QPointF(0, recordingCursorY),
            QPointF(width(), recordingCursorY)
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


// Recording

void PianoRoll::startRecording()
{
    if (m_recording)
        return;

    m_recording = true;

    m_recordingStartTime =
        std::chrono::steady_clock::now();

    m_recordingNoteIndices.clear();

    recordingCursorY = 0.0;

    if (!m_recordingTimer)
    {
        m_recordingTimer =
            new QTimer(this);

        connect(
            m_recordingTimer,
            &QTimer::timeout,
            this,
            [this]()
            {
                if (!m_recording)
                    return;

                const auto now =
                    std::chrono::steady_clock::now();

                const double seconds =
                    std::chrono::duration<double>(
                        now - m_recordingStartTime
                    ).count();

                ensureContentHeight(seconds);

                recordingCursorY =
                    yFromTime(seconds);

                m_recordingLine->setGeometry(
                    0,
                    static_cast<int>(recordingCursorY),
                    width(),
                    1
                );
            }
        );
    }

    m_recordingLine->show();

    m_recordingTimer->start(16);

    setFocus();
    update();
}

void PianoRoll::stopRecording()
{
    if (!m_recording)
        return;

    m_recording = false;

    if (m_recordingTimer)
        m_recordingTimer->stop();

    if (m_recordingLine)
        m_recordingLine->hide();
    recordingCursorY = -1;

    m_recordingNoteIndices.clear();

    update();
}

double PianoRoll::recordingTimeFromTimestamp(
    qint64 timestampNs) const
{
    const auto recordingStartNs =
        std::chrono::duration_cast<
            std::chrono::nanoseconds
        >(
            m_recordingStartTime.time_since_epoch()
        ).count();

    return static_cast<double>(
        timestampNs - recordingStartNs
    ) / 1'000'000'000.0;
}

void PianoRoll::midiNoteOn(
    int midiNote,
    int velocity,
    qint64 timestampNs)
{
    Q_UNUSED(velocity);

    if (!m_recording)
        return;

    if (m_recordingNoteIndices.contains(midiNote))
        return;

    const double time =
        recordingTimeFromTimestamp(timestampNs);

    PlacedNote note;

    note.midiNote = midiNote;
    note.time = std::max(0.0, time);
    note.duration = 0.0;

    pressedNotes.append(note);

    const int noteIndex =
        pressedNotes.size() - 1;

    m_recordingNoteIndices.insert(
        midiNote,
        noteIndex
    );

    selectedNote = noteIndex;

    update();
}

void PianoRoll::midiNoteOff(
    int midiNote,
    qint64 timestampNs)
{
    if (!m_recording)
        return;

    auto it =
        m_recordingNoteIndices.find(midiNote);

    if (it == m_recordingNoteIndices.end())
        return;

    const int noteIndex = it.value();

    if (noteIndex < 0 ||
        noteIndex >= pressedNotes.size())
    {
        m_recordingNoteIndices.erase(it);
        return;
    }

    const double endTime =
        recordingTimeFromTimestamp(timestampNs);

    PlacedNote &note =
        pressedNotes[noteIndex];

    note.duration =
        std::max(
            0.1,
            endTime - note.time
        );

    m_recordingNoteIndices.erase(it);

    update();
}

