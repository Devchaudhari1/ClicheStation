#include "Layer.h"
#include "Clip.h"
#include "ClipArea.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <cmath>
#include <QVector>
Layer::Layer(QWidget *parent)
    : QWidget(parent),
      m_leftWidget(nullptr),
      m_clipArea(nullptr),
      m_muteButton(nullptr),
      m_optionsButton(nullptr)
{
    // ----------------------------------------
    // Left control/card
    // ----------------------------------------

    m_leftWidget =
        new QWidget(nullptr);

    m_leftWidget->setFixedWidth(160);
    m_leftWidget->setFixedHeight(80);

    QVBoxLayout *leftLayout =
        new QVBoxLayout(m_leftWidget);

    leftLayout->setContentsMargins(
        8, 8, 8, 8
    );

    // Keep your existing Layer controls here.
    m_muteButton =
        new QPushButton("M", m_leftWidget);

    m_optionsButton =
        new QPushButton("...", m_leftWidget);

    leftLayout->addWidget(m_muteButton);
    leftLayout->addWidget(m_optionsButton);
    leftLayout->addStretch();

    // ----------------------------------------
    // Timeline
    // ----------------------------------------

    m_clipArea = new ClipArea(nullptr);

    connect(
        m_clipArea,
        &ClipArea::trackDropped,
        this,
        &Layer::trackDropped
    );
}

Clip *Layer::addClip(
    int trackId,
    const QString &trackName,
    double startTime,
    double duration,
    const QVector<float>& samples
)
{
    if (!canAddClip(startTime, duration))
        return nullptr;

    Clip *clip =
        new Clip(
            trackId,
            trackName,
            startTime,
            duration,
            m_clipArea
        );

    clip->setSamples(samples,static_cast<int>(duration * m_pixelsPerSecond));

    connect(
        clip,
        &Clip::moveRequested,
        this,
        [this, clip](double newStartTime)
        {
            if (!canMoveClip(clip, newStartTime))
                return;

            clip->setStartTime(newStartTime);
            layoutClips();
        }
    );

    connect(
        clip,
        &Clip::resizeRequested,
        this,
        [this, clip](
        bool leftEdge,
        double newStartTime,
        double newDuration,
        double newSourceStart,
        double newSourceEnd
    )
        {
            Q_UNUSED(leftEdge);

            if (!canResizeClip(
                    clip,
                    newStartTime,
                    newDuration
                ))
            {
                return;
            }

            clip->setStartTime(newStartTime);
            clip->setDuration(newDuration);

            clip->setSourceRange(
                newSourceStart,
                newSourceEnd
            );

            layoutClips();
        }
    );

    connect(
        clip,
        &Clip::deleteRequested,
        this,
        [this, clip]()
        {
            removeClip(clip);
        }
    );

    m_clips.append(clip);

    emit timelineWidthRequired(
        startTime + duration
    );
    
    layoutClips();

    clip->show();

    emit clipAdded();

    return clip;
}

void Layer::removeClip(Clip *clip)
{
    if (!clip)
        return;

    m_clips.removeOne(clip);

    clip->deleteLater();

    layoutClips();
}


void Layer::layoutClips()
{
    for (Clip *clip : m_clips)
    {
        int x =
            static_cast<int>(
                clip->startTime()
                * m_pixelsPerSecond
            );

        int width =
            static_cast<int>(
                clip->duration()
                * m_pixelsPerSecond
            );

        clip->setGeometry(
            x,
            5,
            width,
            m_clipArea->height() - 10
        );
    }

    m_clipArea->update();
}

bool Layer::canAddClip(
    double startTime,
    double duration
) const
{
    double newEnd =
        startTime + duration;

    for (Clip *clip : m_clips)
    {
        double existingStart =
            clip->startTime();

        double existingEnd =
            existingStart +
            clip->duration();

        bool overlaps =
            startTime < existingEnd &&
            newEnd > existingStart;

        if (overlaps)
            return false;
    }

    return true;
}

bool Layer::canMoveClip(
    const Clip *clip,
    double newStartTime
) const
{
    if (newStartTime < 0.0)
        return false;

    double newEndTime =
        newStartTime + clip->duration();

    for (Clip *other : m_clips)
    {
        if (other == clip)
            continue;

        double otherStart = other->startTime();
        double otherEnd =
            otherStart + other->duration();

        bool overlaps =
            newStartTime < otherEnd &&
            newEndTime > otherStart;

        if (overlaps)
            return false;
    }

    return true;
}

bool Layer::canResizeClip(
    const Clip *clip,
    double newStartTime,
    double newDuration
) const
{
    if (newStartTime < 0.0)
        return false;

    if (newDuration <= 0.0)
        return false;

    double newEndTime =
        newStartTime + newDuration;

    for (Clip *other : m_clips)
    {
        if (other == clip)
            continue;

        double otherStart =
            other->startTime();

        double otherEnd =
            otherStart + other->duration();

        bool overlaps =
            newStartTime < otherEnd &&
            newEndTime > otherStart;

        if (overlaps)
            return false;
    }

    return true;
}


const QVector<Clip*>& Layer::clips() const
{
    return m_clips;
}

QWidget *Layer::leftWidget() const
{
    return m_leftWidget;
}

ClipArea *Layer::clipArea() const
{
    return m_clipArea;
}