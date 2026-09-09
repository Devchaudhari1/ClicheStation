#include "Playlist.h"
#include "TimeRuler.h"
#include "Sequence.h"
#include "ClipArea.h"
#include "Layer.h"
#include "../track_list.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMenu>
#include <QAction>

void Playlist::setAvailableTracks(
    const QVector<QPair<int, QString>> &tracks
)
{
    m_availableTracks = tracks;
    for (Sequence *sequence : m_sequences)
    {
        sequence->setAvailableTracks(m_availableTracks);
    }
}

void Playlist::layoutSequences()
{
    int y = 0;

    const int sequenceHeaderHeight = 40;
    const int layerHeight = 80;

    for (int i = 0; i < m_sequences.size(); ++i)
    {
        Sequence *sequence = m_sequences[i];

        int layerCount = sequence->layers().size();

        int sequenceHeight =
            sequenceHeaderHeight;

        if (sequence->isExpanded())
        {
            sequenceHeight +=
                sequence->layers().size() * layerHeight;
        }

        qDebug() << "Sequence" << i
                 << "height =" << sequenceHeight
                 << "left content width =" << m_leftContent->width()
                 << "right content width =" << m_rightContent->width();

        m_leftSequenceContainers[i]->setGeometry(
            0,
            y,
            m_leftContent->width(),
            sequenceHeight
        );
        
        m_rightSequenceContainers[i]->setGeometry(
            0,
            y,
            m_rightContent->width(),
            sequenceHeight
        );

        y += sequenceHeight;
    }

    m_leftContent->resize(
        m_leftContent->width(),
        y
    );

    m_rightContent->resize(
        m_rightContent->width(),
        y
    );
}

void Playlist::addSequence()
{
    int sequenceId = m_nextSequenceId++;

    Sequence *sequence = new Sequence(sequenceId, this);
    sequence->setAvailableTracks(m_availableTracks);
    m_sequences.append(sequence);

    // --------------------------------------------------
    // LEFT container for this Sequence
    // --------------------------------------------------

    QWidget *leftSequenceContainer = new QWidget(m_leftContent);
    leftSequenceContainer->setStyleSheet(
            "background: #303030;"
            "border: 1px solid red;"
        );
    leftSequenceContainer->show();

    QVBoxLayout *leftSequenceLayout =
        new QVBoxLayout(leftSequenceContainer);

    leftSequenceLayout->setContentsMargins(0, 0, 0, 0);
    leftSequenceLayout->setSpacing(0);


    // --------------------------------------------------
    // RIGHT container for this Sequence
    // --------------------------------------------------

    QWidget *rightSequenceContainer = new QWidget(m_rightContent);
    rightSequenceContainer->setStyleSheet(
        "background: #303030;"
        "border: 1px solid red;"
    );
    rightSequenceContainer->show();

    QVBoxLayout *rightSequenceLayout =
        new QVBoxLayout(rightSequenceContainer);

    rightSequenceLayout->setContentsMargins(0, 0, 0, 0);
    rightSequenceLayout->setSpacing(0);


    // --------------------------------------------------
    // Store the containers/layouts
    // --------------------------------------------------

    m_leftSequenceContainers.append(leftSequenceContainer);
    m_rightSequenceContainers.append(rightSequenceContainer);

    m_leftSequenceLayouts.append(leftSequenceLayout);
    m_rightSequenceLayouts.append(rightSequenceLayout);


    // --------------------------------------------------
    // Sequence header
    // --------------------------------------------------

    leftSequenceLayout->addWidget(
        sequence->headerWidget()
    );
    sequence->headerWidget()->setParent(leftSequenceContainer);
    sequence->headerWidget()->setGeometry(
        0,
        0,
        leftSequenceContainer->width(),
        40
    );
    sequence->headerWidget()->show();
    qDebug() << "HEADER:"
         << sequence->headerWidget()
         << "visible =" << sequence->headerWidget()->isVisible()
         << "geometry =" << sequence->headerWidget()->geometry()
         << "parent =" << sequence->headerWidget()->parentWidget();

    // --------------------------------------------------
    // Matching spacer on right side
    // --------------------------------------------------

    QWidget *spacer = new QWidget;
    spacer->setFixedHeight(40);

    rightSequenceLayout->addWidget(spacer);

    spacer->show();


    // --------------------------------------------------
    // First Layer
    //
    // Sequence creates its first Layer in its constructor,
    // before Playlist connects to layerAdded().
    // Therefore we add this one manually.
    // --------------------------------------------------

    if (!sequence->layers().isEmpty())
    {
        Layer *layer = sequence->layers().first();

        leftSequenceLayout->addWidget(
            layer->leftWidget()
        );

        rightSequenceLayout->addWidget(
            layer->clipArea()
        );

        layer->leftWidget()->show();
        layer->clipArea()->show();
        
    qDebug() << "LEFT LAYER:"
         << layer->leftWidget()
         << "visible =" << layer->leftWidget()->isVisible()
         << "geometry =" << layer->leftWidget()->geometry()
         << "parent =" << layer->leftWidget()->parentWidget();

qDebug() << "CLIP AREA:"
         << layer->clipArea()
         << "visible =" << layer->clipArea()->isVisible()
         << "geometry =" << layer->clipArea()->geometry()
         << "parent =" << layer->clipArea()->parentWidget();
    }

    // --------------------------------------------------
    // Future Layers
    // --------------------------------------------------

    connect(
        sequence,
        &Sequence::layerAdded,
        this,
        [this,
        sequence,
        leftSequenceLayout,
        rightSequenceLayout]()
        {
            Layer *layer = sequence->layers().last();

            leftSequenceLayout->addWidget(
                layer->leftWidget()
            );

            rightSequenceLayout->addWidget(
                layer->clipArea()
            );

            layer->leftWidget()->setVisible(
                sequence->isExpanded()
            );

            layer->clipArea()->setVisible(
                sequence->isExpanded()
            );

            layoutSequences();
        }
    );

    connect(
        sequence,
        &Sequence::expandedChanged,
        this,
        [this, sequence](bool expanded)
        {
            for (Layer *layer : sequence->layers())
            {
                layer->leftWidget()->setVisible(expanded);
                layer->clipArea()->setVisible(expanded);
            }

            layoutSequences();
        }
    );


    // --------------------------------------------------
    // Calculate and position all Sequences
    // --------------------------------------------------

    layoutSequences();

    // --------------------------------------------------
    // Debug
    // --------------------------------------------------

    qDebug() << "Added Sequence" << sequenceId;

    qDebug() << "LEFT CONTENT:"
             << m_leftContent->size()
             << "sizeHint:"
             << m_leftContent->sizeHint();

    qDebug() << "RIGHT CONTENT:"
             << m_rightContent->size()
             << "sizeHint:"
             << m_rightContent->sizeHint();

    qDebug() << "LEFT PANE:"
             << m_leftPane->size();

    qDebug() << "RIGHT PANE:"
             << m_rightPane->size();
}


static QWidget *makeTestWidget(
    const QString &text,
    int height,
    const QString &color,
    QWidget *parent = nullptr)
{
    QWidget *widget = new QWidget(parent);

    widget->setFixedHeight(height);

    widget->setStyleSheet(
        QString(
            "background: %1;"
            "border: 1px solid black;"
        ).arg(color)
    );

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(4, 4, 4, 4);

    QLabel *label = new QLabel(text, widget);
    layout->addWidget(label);

    return widget;
}

Playlist::Playlist(QWidget *parent)
    : QWidget(parent)
{
    createUI();
   
}


void Playlist::createUI()
{
    QVBoxLayout *playlistLayout = new QVBoxLayout(this);
    playlistLayout->setContentsMargins(0, 0, 0, 0);
    playlistLayout->setSpacing(0);

    // =========================================================
    // Time Ruler
    // =========================================================

    m_timeRuler = new TimeRuler(this);
    playlistLayout->addWidget(m_timeRuler);


    // =========================================================
    // Timeline View
    // =========================================================

    m_timelineView = new QWidget(this);
    m_timelineView->setStyleSheet(
        "background: black;"
    );

    QHBoxLayout *timelineLayout =
        new QHBoxLayout(m_timelineView);

    timelineLayout->setContentsMargins(0, 0, 0, 0);
    timelineLayout->setSpacing(0);


    // =========================================================
    // Left Pane
    // =========================================================

    m_leftPane = new QScrollArea(m_timelineView);

    m_leftPane->setFixedWidth(220);

    m_leftPane->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_leftPane->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_leftPane->setWidgetResizable(false);


    // ---------------------------------------------------------
    // Left Content
    // ---------------------------------------------------------

    m_leftContent = new QWidget;
    m_leftContent->setFixedWidth(220);
    m_leftContent->setMinimumHeight(1);
    m_leftContent->setStyleSheet(
        "background: #202020;"
    );

    // =========================================================
    // Right Pane
    // =========================================================

    m_rightPane = new QScrollArea(m_timelineView);

    m_rightPane->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    m_rightPane->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    m_rightPane->setWidgetResizable(false);

    connect(
    m_rightPane->horizontalScrollBar(),
    &QScrollBar::valueChanged,
    m_timeRuler,
    [this](int value)
    {
        m_timeRuler->setScrollOffset(value);
    }
    );
    // Two way schronization with a boolean guard connections 
    // for synchronized vertical scrolling
    connect(
        m_leftPane->verticalScrollBar(),
        &QScrollBar::valueChanged,
        this,
        [this](int value)
        {
            if (m_syncingVerticalScroll)
                return;

            m_syncingVerticalScroll = true;

            m_rightPane->verticalScrollBar()->setValue(value);

            m_syncingVerticalScroll = false;
        }
    );

    connect(
        m_rightPane->verticalScrollBar(),
        &QScrollBar::valueChanged,
        this,
        [this](int value)
        {
            if (m_syncingVerticalScroll)
                return;

            m_syncingVerticalScroll = true;

            m_leftPane->verticalScrollBar()->setValue(value);

            m_syncingVerticalScroll = false;
        }
    );
    // ---------------------------------------------------------
    // Right Content
    // ---------------------------------------------------------

    m_rightContent = new QWidget;

    m_rightContent->setMinimumWidth(2000);
    m_rightContent->setMinimumHeight(1);
    m_rightContent->setStyleSheet(
        "background: #202020;"
    );


    // =========================================================
    // Install Content
    // =========================================================

    m_leftPane->setWidget(m_leftContent);
    m_rightPane->setWidget(m_rightContent);

    // =========================================================
    // Add Timeline View to Playlist
    // =========================================================

     timelineLayout->addWidget(m_leftPane);
    timelineLayout->addWidget(m_rightPane, 1);

    playlistLayout->addWidget(m_timelineView, 1);
    // // =========================================================
    // // TEMPORARY TEST CONTENT
    // // =========================================================

    // const int sequenceHeight = 40;
    // const int layerHeight = 70;

    // for (int sequence = 1; sequence <= 3; ++sequence)
    // {
    //     // Sequence header - left
    //     m_leftLayout->addWidget(
    //         makeTestWidget(
    //             QString("Sequence %1").arg(sequence),
    //             sequenceHeight,
    //             "orange",
    //             m_leftContent
    //         )
    //     );

    //     // Sequence header - right
    //     m_rightLayout->addWidget(
    //         makeTestWidget(
    //             QString("Sequence %1").arg(sequence),
    //             sequenceHeight,
    //             "orange",
    //             m_rightContent
    //         )
    //     );

    //     int layerCount = sequence + 2;

    //     for (int layer = 1; layer <= layerCount; ++layer)
    //     {
    //         // Layer - left
    //         m_leftLayout->addWidget(
    //             makeTestWidget(
    //                 QString("Layer %1").arg(layer),
    //                 layerHeight,
    //                 "steelblue",
    //                 m_leftContent
    //             )
    //         );

    //         // ClipArea - right
    //         m_rightLayout->addWidget(
    //             makeTestWidget(
    //                 QString("Layer %1 ClipArea").arg(layer),
    //                 layerHeight,
    //                 "seagreen",
    //                 m_rightContent
    //             )
    //         );
    //     }
    // }
    // m_rightContent->setMinimumWidth(3000);

    // m_leftContent->adjustSize();
    // m_rightContent->adjustSize();

}