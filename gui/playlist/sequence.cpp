#include "sequence.h"
#include "layer.h"
#include "clip.h"
#include "TimelineRow.h"
#include "ClipArea.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>

Sequence::Sequence(
    int sequenceId,
    QWidget *parent
)
    : QWidget(parent),
      m_sequenceId(sequenceId),
      m_header(nullptr),
      m_headerSpacer(nullptr),
      m_expandButton(nullptr),
      m_sequenceLabel(nullptr),
      m_addLayerButton(nullptr),
      m_layerContainer(nullptr),
      m_layerLayout(nullptr)
{
    QVBoxLayout *mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        0, 0, 0, 0
    );

    mainLayout->setSpacing(0);
        setSizePolicy(
        QSizePolicy::Preferred,
        QSizePolicy::Preferred
    );
    // mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    // -------------------------------------------------
    // Sequence header
    // -------------------------------------------------

    m_header =
        new QWidget(this);

    QHBoxLayout *headerLayout =
        new QHBoxLayout(m_header);

    headerLayout->setContentsMargins(
        6, 3, 6, 3
    );

    headerLayout->setSpacing(4);

    // Expand/collapse button
    m_expandButton =
        new QPushButton("▼", m_header);

    m_expandButton->setFixedWidth(28);

    headerLayout->addWidget(
        m_expandButton
    );

    // Sequence name
    m_sequenceLabel =
        new QLabel(
            QString("Sequence %1")
                .arg(m_sequenceId),
            m_header
        );

    headerLayout->addWidget(
        m_sequenceLabel
    );

    // Push Add Layer to the right
    headerLayout->addStretch();

    m_addLayerButton =
        new QPushButton(
            "+ Add Layer",
            m_header
        );

    headerLayout->addWidget(
        m_addLayerButton
    );

    mainLayout->addWidget(
        m_header
    );
    m_headerSpacer =
    new QWidget(this);

    m_headerSpacer->setFixedHeight(40);

    mainLayout->addWidget(
        m_headerSpacer
    );
    // -------------------------------------------------
    // Layer container
    // -------------------------------------------------

    m_layerContainer =
        new QWidget(this);
    m_layerContainer->setStyleSheet(
    "background: orange; border: 4px solid red;"
    );
    m_layerLayout =
        new QVBoxLayout(m_layerContainer);

    m_layerLayout->setContentsMargins(
        0, 0, 0, 0
    );

    m_layerLayout->setSpacing(0);


    mainLayout->addWidget(
        m_layerContainer,0
    );

    // -------------------------------------------------
    // Connections
    // -------------------------------------------------

    connect(
        m_expandButton,
        &QPushButton::clicked,
        this,
        &Sequence::toggleExpanded
    );

    connect(
        m_addLayerButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            addLayer();
        }
    );

    // Start with one layer
    addLayer();
}

int Sequence::sequenceId() const
{
    return m_sequenceId;
}

void Sequence::toggleExpanded()
{
    m_expanded = !m_expanded;

    m_layerContainer->setVisible(m_expanded);

    m_expandButton->setText(
        m_expanded ? "▼" : "▶"
    );

    updateGeometry();

    emit expandedChanged(m_expanded);
}

Layer *Sequence::addLayer()
{
    TimelineRow *row =
        new TimelineRow(m_layerContainer);

    Layer *layer =
        new Layer(row);

    
    connect(
    layer,
    &Layer::clipRequested,
    this,
    [this, layer]()
    {
        QMenu menu;

        for (const auto &track : m_availableTracks)
        {
            QAction *action = menu.addAction(track.second);
            action->setData(track.first);
        }

        if (menu.isEmpty())
            return;

        QAction *selected = menu.exec(
            layer->mapToGlobal(
                QPoint(layer->width(), layer->height())
            )
        );

        if (!selected)
            return;

        int trackId = selected->data().toInt();
        QString trackName = selected->text();

        double startTime = 0.0;
        double duration = 4.0;

        const auto &clips = layer->clips();

        if (!clips.isEmpty())
        {
            Clip *lastClip = clips.last();
            startTime =
                lastClip->startTime() +
                lastClip->duration();
        }

        layer->addClip(
            trackId,
            trackName,
            startTime,
            duration
        );
    }
);

connect(
    layer,
    &Layer::trackDropped,
    this,
    [this, layer](int trackId, double startTime)
    {
        QString trackName;

        for (const auto &track : m_availableTracks)
        {
            if (track.first == trackId)
            {
                trackName = track.second;
                break;
            }
        }

        if (trackName.isEmpty())
            return;

        double duration = 1.0; // temporary

        layer->addClip(
            trackId,
            trackName,
            startTime,
            duration
        );
    });
    row->setLeftWidget(
        layer->leftWidget()
    );

    row->setTimelineWidget(
        layer->clipArea()
    );

    int insertPosition =
        m_layerLayout->count();

    m_layerLayout->insertWidget(insertPosition, row);

    m_layerContainer->adjustSize();
    updateGeometry();

    emit layerAdded();
    return layer;
}

void Sequence::setAvailableTracks(
    const QVector<QPair<int, QString>> &tracks
)
{
    m_availableTracks = tracks;
}

QWidget *Sequence::layerContainer() const
{
    return m_layerContainer;
}

QWidget *Sequence::headerWidget() const
{
    return m_header;
}

QVector<TimelineRow *> Sequence::layerRows() const
{
    QVector<TimelineRow *> rows;

    for (int i = 0; i < m_layerLayout->count(); ++i)
    {
        QWidget *widget =
            m_layerLayout->itemAt(i)->widget();

        TimelineRow *row =
            qobject_cast<TimelineRow *>(widget);

        if (row)
            rows.append(row);
    }

    return rows;
}

void Sequence::detachHeader()
{
    QVBoxLayout *layout =
        qobject_cast<QVBoxLayout*>(this->layout());

    if (!layout)
        return;

    layout->removeWidget(
        m_header
    );
}