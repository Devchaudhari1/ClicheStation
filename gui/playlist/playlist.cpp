#include "playlist.h"
#include "sequence.h"
#include "TimeRuler.h"
#include "TimelineRow.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>

Playlist::Playlist(QWidget *parent)
    : QWidget(parent),
      m_sequenceLayout(nullptr),
      m_timelineBody(nullptr),
      m_leftColumn(nullptr),
      m_leftContent(nullptr),
      m_leftLayout(nullptr), 
      m_scrollArea(nullptr),
      m_timelineContent(nullptr),
      m_horizontalScrollBar(nullptr),
      m_timeRuler(nullptr)
{
    QVBoxLayout *mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        0, 0, 0, 0
    );

    mainLayout->setSpacing(0);
    
    // ----------------------------------------
    // Shared time ruler
    // ----------------------------------------

    m_timeRuler =
        new TimeRuler(this);

    mainLayout->addWidget(
        m_timeRuler
    );

    // ----------------------------------------
    // Scrollable timeline
    // ----------------------------------------

    m_timelineBody =
    new QWidget(this);

    m_timelineBody->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding);

    QHBoxLayout *timelineBodyLayout =
        new QHBoxLayout(m_timelineBody);

    timelineBodyLayout->setContentsMargins(
        0, 0, 0, 0
    );

    timelineBodyLayout->setSpacing(0);

    m_leftColumn =
    new QWidget(m_timelineBody);

    m_leftColumn->setFixedWidth(160);

    m_leftColumn =
    new QWidget(m_timelineBody);

    m_leftColumn->setFixedWidth(160);

    m_leftLayout =
        new QVBoxLayout(m_leftColumn);

    m_leftLayout->setContentsMargins(
        0, 0, 0, 0
    );

    m_leftLayout->setSpacing(0);

    QLabel *leftLabel =
        new QLabel(
            "Layers",
            m_leftColumn
        );

    leftLabel->setAlignment(
        Qt::AlignCenter
    );

    m_leftLayout->addWidget(
        leftLabel
    );

    timelineBodyLayout->addWidget(
        m_leftColumn
    );

    m_scrollArea =
        new QScrollArea(m_timelineBody);

    m_scrollArea->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    m_scrollArea->setWidgetResizable(true);

    m_scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    m_scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    m_timelineContent =
        new QWidget;
    

    m_timelineContent->setMinimumHeight(0);

    m_sequenceLayout =
        new QVBoxLayout(m_timelineContent);
    
    m_sequenceLayout->setContentsMargins(
        0, 0, 0, 0
    );

    m_sequenceLayout->setSpacing(0);

    m_sequenceLayout->setSizeConstraint(
        QLayout::SetMinimumSize);

    m_scrollArea->setWidget(
        m_timelineContent
    );

    m_horizontalScrollBar =
        m_scrollArea->horizontalScrollBar();

    timelineBodyLayout->addWidget(
        m_scrollArea,
        1
    );

    connect(
    m_horizontalScrollBar,
    &QScrollBar::valueChanged,
    this,
    [this](int value)
    {
        m_timeRuler->setScrollOffset(
            value
        );
    }
    );

    mainLayout->addWidget(
        m_timelineBody
    );

    // ----------------------------------------
    // Default Sequence
    // ----------------------------------------

    addSequence();
}
Sequence *Playlist::addSequence()
{
    int sequenceId =
        m_nextSequenceId++;

    Sequence *sequence =
        new Sequence(
            sequenceId,
            m_timelineContent
        );

    sequence->setAvailableTracks(
        m_availableTracks
    );

    // -------------------------------------------------
    // Create a left-side container for this Sequence
    // -------------------------------------------------

    QWidget *sequenceLeftContainer =
        new QWidget(m_leftColumn);

    QVBoxLayout *sequenceLeftLayout =
        new QVBoxLayout(sequenceLeftContainer);

    sequenceLeftLayout->setContentsMargins(
        0, 0, 0, 0
    );

    sequenceLeftLayout->setSpacing(0);

    m_leftLayout->addWidget(
        sequenceLeftContainer
    );

    // -------------------------------------------------
    // Move Sequence header into its container
    // -------------------------------------------------

    sequence->detachHeader();

    QWidget *header =
        sequence->headerWidget();

    header->setParent(
        sequenceLeftContainer
    );

    header->setFixedWidth(
        160
    );

    sequenceLeftLayout->addWidget(
        header
    );

    header->show();

    // -------------------------------------------------
    // Move the already-existing first layer
    // -------------------------------------------------

    const auto existingRows =
        sequence->layerRows();

    for (TimelineRow *row : existingRows)
    {
        if (!row)
            continue;

        QWidget *leftWidget =
            row->leftWidget();

        if (!leftWidget)
            continue;

        leftWidget->setParent(
            sequenceLeftContainer
        );

        sequenceLeftLayout->addWidget(
            leftWidget
        );

        leftWidget->show();
    }

    // -------------------------------------------------
    // Handle future layers
    // -------------------------------------------------

    connect(
        sequence,
        &Sequence::layerAdded,
        this,
        [sequence, sequenceLeftContainer]()
        {
            QVBoxLayout *layout =
                qobject_cast<QVBoxLayout*>(
                    sequenceLeftContainer->layout()
                );

            if (!layout)
                return;

            const auto rows =
                sequence->layerRows();

            if (rows.isEmpty())
                return;

            TimelineRow *row =
                rows.last();

            if (!row)
                return;

            QWidget *leftWidget =
                row->leftWidget();

            if (!leftWidget)
                return;

            leftWidget->setParent(
                sequenceLeftContainer
            );

            layout->addWidget(
                leftWidget
            );

            leftWidget->show();
        }
    );

    // -------------------------------------------------
    // Handle Sequence collapse / expand
    // -------------------------------------------------

    connect(
        sequence,
        &Sequence::expandedChanged,
        this,
        [sequence](bool expanded)
        {
            const auto rows =
                sequence->layerRows();

            for (TimelineRow *row : rows)
            {
                if (!row)
                    continue;

                QWidget *leftWidget =
                    row->leftWidget();

                if (leftWidget)
                {
                    leftWidget->setVisible(
                        expanded
                    );
                }
            }
        }
    );

    // -------------------------------------------------
    // Add Sequence to scrolling timeline
    // -------------------------------------------------

    m_sequenceLayout->addWidget(sequence);

    m_lastSequence =
        sequence;

    return sequence;
}

void Playlist::setAvailableTracks(
    const QVector<QPair<int, QString>> &tracks
)
{
    m_availableTracks = tracks;

    // Existing sequences
    for (int i = 0;
         i < m_sequenceLayout->count();
         ++i)
    {
        QWidget *widget =
            m_sequenceLayout->itemAt(i)->widget();

        Sequence *sequence =
            qobject_cast<Sequence*>(widget);

        if (sequence)
        {
            sequence->setAvailableTracks(
                m_availableTracks
            );
        }
    }
}