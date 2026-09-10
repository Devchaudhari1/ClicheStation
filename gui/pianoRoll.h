#pragma once

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <QStack>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTimer>
#include <QHash>
#include <chrono>

class QFrame;

struct PlacedNote
{
    int midiNote;
    double time;
    double duration;
};

struct PianoAction
{
    QVector<PlacedNote> before;
    QVector<PlacedNote> after;
};

class PianoRoll : public QWidget
{
    Q_OBJECT

public:
    explicit PianoRoll(QWidget *parent = nullptr);
    const QVector<PlacedNote>& notes() const;
    void startRecording();
    void stopRecording();

    void midiNoteOn(
        int midiNote,
        int velocity,
        qint64 timestampNs
    );
    void ensureContentHeight(double time);
    void midiNoteOff(
        int midiNote,
        qint64 timestampNs
    );
signals:
    void becameActive(PianoRoll* pianoRoll);
    void requestScrollToY(int y);
    void requestScrollBy(int delta);

protected:
    void paintEvent(QPaintEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void leaveEvent(QEvent *event) override;

private:
    static constexpr int firstMidiNote = 21;
    static constexpr int lastMidiNote = 108;
    static constexpr int keyCount = 88;

    double pixelsPerSecond = 100.0;
    double defaultDuration = 0.5;

    double recordingTimeFromTimestamp(
        qint64 timestampNs
    ) const;
    QVector<PlacedNote> pressedNotes;
    // Recording States
    bool m_recording = false;

    QFrame *m_recordingLine = nullptr;

    QTimer *m_recordingTimer = nullptr;

    std::chrono::steady_clock::time_point
        m_recordingStartTime;

    double recordingCursorY = -1;
    double m_contentHeight;
    double m_lastContentGrowthTime = 0.0;
    double m_originY;
    QHash<int, int> m_recordingNoteIndices;
    // Copy/paste
    QVector<PlacedNote> copiedNotes;

    // Undo/Redo
    QStack<PianoAction> undoStack;
    QStack<PianoAction> redoStack;

    // Hover
    int hoveredKey = -1;
    double cursorY = -1;

    // Selection
    int selectedNote = -1;

    // Dragging
    bool draggingNote = false;
    bool resizingNote = false;

    double dragOffsetY = 0.0;

    int keyFromX(double x) const;

    double timeFromY(double y) const;
    double yFromTime(double time) const;
    int noteAtPosition(
        const QPointF &position
    ) const;

    bool isResizeArea(
        const QPointF &position,
        int noteIndex
    ) const;

    void drawNote(
        QPainter &painter,
        const PlacedNote &note,
        bool selected
    );

    void copySelectedNote();
    void pasteCopiedNote();

    // Undo/Redo
    void saveAction(
        const QVector<PlacedNote> &before
    );

    void undo();
    void redo();
};