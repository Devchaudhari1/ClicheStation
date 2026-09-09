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
        const QString &trackName,
        double startTime,
        double duration,
        QWidget *parent = nullptr
    );

    int trackId() const;
    QString trackName() const;
    double startTime() const;
    double duration() const;

    QString endTimeText() const;

    void setStartTime(double startTime);
    void setDuration(double duration);
    void setSamples(const QVector<float> &samples);
signals:
    void moveRequested(double newStartTime);
    void resizeRequested(
        bool leftEdge,
        double newStartTime,
        double newDuration
    );
    void deleteRequested();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    void createUI();

    bool m_dragging = false;
    bool m_resizing = false;
    bool m_resizeLeft = false;

    QPoint m_dragStartPosition;
    double m_originalStartTime = 0.0;
    double m_originalDuration = 0.0;
    
    int m_trackId;
    QString m_trackName;
    
    double m_startTime;
    double m_duration;

    QLabel *m_trackLabel;

    ClipVisualizer *m_visualizer;
};