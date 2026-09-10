#include "track_list.h"
#include "voice_track.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QCursor>
#include <QHBoxLayout>
#include <QPushButton>
#include "../audio/AudioEngine.h"
TrackList::TrackList(AudioEngine *audioEngine, QWidget *parent)
    : QWidget(parent),
      m_audioEngine(audioEngine),
      m_trackLayout(nullptr),
      m_addTrackButton(nullptr)
{
    createUI();
}

void TrackList::createUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_handle = new TrackListHandle(this);
    mainLayout->addWidget(m_handle);

    QWidget *trackArea = new QWidget(this);

    m_trackLayout = new QHBoxLayout(trackArea);
    m_trackLayout->setContentsMargins(10, 8, 10, 8);
    m_trackLayout->setSpacing(8);
    m_trackLayout->setAlignment(Qt::AlignTop);
    
    m_addTrackButton =
        new QPushButton("+ Add Voice Track", trackArea);

    m_trackLayout->addWidget(m_addTrackButton);
    m_trackLayout->addStretch();

    mainLayout->addWidget(trackArea);

    setStyleSheet(R"(
        QWidget {
            background-color: #171717;
            color: white;
        }

        QPushButton {
            background-color: #2b2b2b;
            color: white;
            border: 1px solid #444444;
            border-radius: 6px;
            padding: 6px 12px;
        }

        QPushButton:hover {
            background-color: #3a3a3a;
        }

        QPushButton:pressed {
            background-color: #222222;
        }
    )");
    connect(
        m_addTrackButton,
        &QPushButton::clicked,
        this,
        &TrackList::addVoiceTrack
    );

    connect(
        m_handle,
        &TrackListHandle::dragDelta,
        this,
        [this](int delta)
        {
            QWidget *parent = parentWidget();

            if (!parent)
                return;

            int newHeight = height() - delta;

            newHeight = qBound(
                100,
                newHeight,
                parent->height()
            );

            setGeometry(
                0,
                parent->height() - newHeight,
                parent->width(),
                newHeight
            );
        }
    );
}

VoiceTrack *TrackList::findVoiceTrack(int trackId) const
{
    for (VoiceTrack *track : findChildren<VoiceTrack*>())
    {
        if (track->trackId() == trackId)
            return track;
    }

    return nullptr;
}

void TrackList::addVoiceTrack()
{
    int trackId = m_nextTrackId++;

    QString trackName =
        QString("Voice Track %1").arg(trackId);

    // Create the corresponding audio processor
    if (m_audioEngine)
    {
        m_audioEngine->createTrack(trackId);
    }

    VoiceTrack *track =
        new VoiceTrack(trackId, trackName, m_audioEngine, this);

    int insertPosition = m_trackLayout->count() - 1;

    m_trackLayout->insertWidget(insertPosition, track);

    m_tracks.append(qMakePair(trackId, trackName));

    emit tracksChanged(m_tracks);
}

QVector<QPair<int, QString>> TrackList::availableTracks() const
{
    return m_tracks;
}

TrackListHandle::TrackListHandle(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(24);
    setCursor(Qt::SizeVerCursor);
    setMouseTracking(true);
}

void TrackListHandle::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;
        m_lastY = event->globalPosition().toPoint().y();
        event->accept();
    }
}

void TrackListHandle::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dragging)
        return;

    int currentY = event->globalPosition().toPoint().y();
    int delta = currentY - m_lastY;

    if (delta != 0)
    {
        emit dragDelta(delta);
        m_lastY = currentY;
    }

    event->accept();
}

void TrackListHandle::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        event->accept();
    }
}

void TrackListHandle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setPen(QPen(QColor("#666666"), 2));

    const int gripWidth = 60;
    const int gripHeight = 4;

    QRect grip(
        (width() - gripWidth) / 2,
        (height() - gripHeight) / 2,
        gripWidth,
        gripHeight
    );

    painter.drawRoundedRect(grip, 2, 2);
}