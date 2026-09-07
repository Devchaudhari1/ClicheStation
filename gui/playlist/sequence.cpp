#include "sequence.h"
#include "layer.h"
#include "clip.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>

Sequence::Sequence(
    int sequenceId,
    QWidget *parent
)
    : QWidget(parent),
    
      m_sequenceId(sequenceId),
      m_header(nullptr),
      m_expandButton(nullptr),
      m_sequenceLabel(nullptr),
      m_addLayerButton(nullptr)
{
    // -------------------------------------------------
    // Sequence header
    // -------------------------------------------------

    m_header =
        new QWidget(nullptr);

    m_header->setFixedHeight(40);

    m_header->setStyleSheet(
        "background: #3a3a3a;"
        "border: 1px solid #555555;"
    );
    QHBoxLayout *headerLayout =
        new QHBoxLayout(m_header);

    headerLayout->setContentsMargins(
        6, 3, 6, 3
    );

    headerLayout->setSpacing(4);

    m_expandButton =
        new QPushButton("▼", m_header);

    m_expandButton->setFixedWidth(28);

    headerLayout->addWidget(
        m_expandButton
    );

    m_sequenceLabel =
        new QLabel(
            QString("Sequence %1")
                .arg(m_sequenceId),
            m_header
        );
    
    m_sequenceLabel->setStyleSheet(
        "color: #e0e0e0;"
    );
    headerLayout->addWidget(
        m_sequenceLabel
    );

    headerLayout->addStretch();

    m_addLayerButton =
        new QPushButton(
            "+ Add Layer",
            m_header
        );
    m_expandButton->setStyleSheet(
        "background: #4a4a4a;"
        "color: #e0e0e0;"
        "border: 1px solid #666666;"
    );

    m_addLayerButton->setStyleSheet(
        "background: #4a4a4a;"
        "color: #e0e0e0;"
        "border: 1px solid #666666;"
    );
    headerLayout->addWidget(
        m_addLayerButton
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

    m_expandButton->setText(
        m_expanded ? "▼" : "▶"
    );

    emit expandedChanged(m_expanded);
}

Layer *Sequence::addLayer()
{
    Layer *layer =
        new Layer(nullptr);

    connect(
        layer,
        &Layer::clipRequested,
        this,
        [this, layer]()
        {
            QMenu menu;

            for (const auto &track : m_availableTracks)
            {
                QAction *action =
                    menu.addAction(track.second);

                action->setData(track.first);
            }

            if (menu.isEmpty())
                return;

            QAction *selected =
                menu.exec(
                    layer->mapToGlobal(
                        QPoint(
                            layer->width(),
                            layer->height()
                        )
                    )
                );

            if (!selected)
                return;

            int trackId =
                selected->data().toInt();

            QString trackName =
                selected->text();

            double startTime = 0.0;
            double duration = 4.0;

            const auto &clips =
                layer->clips();

            if (!clips.isEmpty())
            {
                Clip *lastClip =
                    clips.last();

                startTime =
                    lastClip->startTime()
                    + lastClip->duration();
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
        [this, layer](
            int trackId,
            double startTime
        )
        {
            QString trackName;

            for (const auto &track : m_availableTracks)
            {
                if (track.first == trackId)
                {
                    trackName =
                        track.second;

                    break;
                }
            }

            if (trackName.isEmpty())
                return;

            double duration = 1.0;

            layer->addClip(
                trackId,
                trackName,
                startTime,
                duration
            );
        }
    );

    m_layers.append(layer);

    emit layerAdded();

    return layer;
}

void Sequence::setAvailableTracks(
    const QVector<QPair<int, QString>> &tracks
)
{
    m_availableTracks = tracks;
}

QWidget *Sequence::headerWidget() const
{
    return m_header;
}


const QVector<Layer *> &Sequence::layers() const
{
    return m_layers;
}

bool Sequence::isExpanded() const
{
    return m_expanded;
}