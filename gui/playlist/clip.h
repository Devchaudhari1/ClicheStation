#pragma once

#include <QWidget>
#include <QString>
#include <QVector>

class QLabel;
class QMouseEvent;
class QPaintEvent;
class ClipVisualizer;

class Clip : public QWidget
{
    Q_OBJECT

public:
    explicit Clip(
        int trackId,
        const QString& trackName,
        double startTime,
        double duration,
        QWidget* parent = nullptr
    );

    int trackId() const;
    QString trackName() const;

    double startTime() const;
    double duration() const;

    double sourceStart() const;
    double sourceEnd() const;
    double fullSourceDuration() const;
    void setSourceRange(double sourceStart, double sourceEnd);
    
    QString endTimeText() const;

    void setStartTime(double startTime);
    void setDuration(double duration);

    // Called once when the clip is created/dropped.
    void setSamples(
        const QVector<float>& samples,
        int fullWidth
    );

signals:
    void moveRequested(double newStartTime);

    void resizeRequested(
        bool leftEdge,
        double newStartTime,
        double newDuration,
        double newSourceStart,
        double newSourceEnd
    );


    void deleteRequested();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void createUI();

private:
    // Interaction state
    bool m_dragging = false;
    bool m_resizing = false;
    bool m_resizeLeft = false;

    QPoint m_dragStartPosition;

    double m_originalStartTime = 0.0;
    double m_originalDuration = 0.0;

    double m_originalSourceStart = 0.0;
    double m_originalSourceEnd = 0.0;

    // Track information
    int m_trackId;
    QString m_trackName;

    // Timeline range
    double m_startTime = 0.0;
    double m_duration = 0.0;

    // Source range inside the frozen render
    double m_sourceStart = 0.0;
    double m_sourceEnd = 0.0;

    // Never changes after the clip's audio is rendered
    double m_fullSourceDuration = 0.0;
    

    QLabel* m_trackLabel = nullptr;
    ClipVisualizer* m_visualizer = nullptr;
};