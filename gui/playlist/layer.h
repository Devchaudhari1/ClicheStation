#pragma once

#include <QWidget>
#include <QVector>

class QPushButton;
class Clip;
class ClipArea;

class Layer : public QWidget
{
    Q_OBJECT

public:
    explicit Layer(QWidget *parent = nullptr);

    Clip *addClip(
        int trackId,
        const QString &trackName,
        double startTime,
        double duration
    );

    bool canAddClip(double startTime, double duration) const;
    bool canMoveClip(const Clip *clip, double newStartTime) const;
    bool canResizeClip(
        const Clip *clip,
        double newStartTime,
        double newDuration
    ) const;

    const QVector<Clip*>& clips() const;

    QWidget *leftWidget() const;
    ClipArea *clipArea() const;

signals:
    void clipRequested();
    void trackDropped(int trackId, double startTime);
    void clipAdded();

private:
    void layoutClips();

    QWidget *m_leftWidget;
    ClipArea *m_clipArea;

    QPushButton *m_muteButton;
    QPushButton *m_optionsButton;

    QVector<Clip*> m_clips;

    double m_pixelsPerSecond = 100.0;
};