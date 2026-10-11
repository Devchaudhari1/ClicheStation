#include "TimelineView.h"
#include "TimeRuler.h"
#include "Layer.h"
#include "ClipArea.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

void TimelineView::ensureWidthForTime(double endTime)
{
    const double paddingSeconds = 2.0;

    const int requiredWidth =
        static_cast<int>(
            (endTime + paddingSeconds) * 100.0
        );

    if (requiredWidth <= m_rightContent->width())
        return;

    m_rightContent->setMinimumWidth(
        requiredWidth
    );

    m_rightContent->resize(
        requiredWidth,
        m_rightContent->height()
    );

    m_timeRuler->setFixedWidth(
        requiredWidth
    );

    layoutSequences();
}

TimelineView::TimelineView(QWidget *parent)
    : QWidget(parent),
      m_verticalScrollArea(nullptr),
      m_horizontalScrollArea(nullptr),
      m_timelineContent(nullptr),
      m_leftContent(nullptr),
      m_rightContent(nullptr),
      m_rightContainer(nullptr),
      m_leftTopPadding(nullptr),
      m_horizontalScrollBar(nullptr),
      m_nativeHorizontalScrollBar(nullptr),
      m_timeRuler(nullptr)
{
    QVBoxLayout *mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --------------------------------------------------
    // ONE vertical scroll area
    // --------------------------------------------------

    m_verticalScrollArea =
        new QScrollArea(this);

    m_verticalScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_verticalScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    m_verticalScrollArea->setWidgetResizable(true);

    // --------------------------------------------------
    // Timeline content
    // --------------------------------------------------

    m_timelineContent =
        new QWidget;

    QHBoxLayout *timelineLayout =
        new QHBoxLayout(m_timelineContent);

    timelineLayout->setContentsMargins(0, 0, 0, 0);
    timelineLayout->setSpacing(0);

    // --------------------------------------------------
    // Left content
    // --------------------------------------------------

    m_leftContent =
        new QWidget;

    m_leftContent->setFixedWidth(160);

    m_leftContent->setStyleSheet(
        "background: transparent;"
    );

    QVBoxLayout *leftLayout =
        new QVBoxLayout(m_leftContent);

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // --------------------------------------------------
    // Right horizontal scroll area
    // --------------------------------------------------

    m_horizontalScrollArea =
        new QScrollArea;

    m_horizontalScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_horizontalScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_horizontalScrollArea->setSizeAdjustPolicy(
        QAbstractScrollArea::AdjustIgnored
    );

    m_horizontalScrollArea->setWidgetResizable(false);

    // --------------------------------------------------
    // Right content
    // --------------------------------------------------

    m_rightContent =
        new QWidget;

    m_rightContent->setMinimumWidth(7000);

    m_rightContent->setStyleSheet(
        "background: #093009;"
    );

    // --------------------------------------------------
    // Native horizontal scrollbar
    // --------------------------------------------------

    m_nativeHorizontalScrollBar =
        m_horizontalScrollArea->horizontalScrollBar();

    // --------------------------------------------------
    // Visible horizontal scrollbar
    // --------------------------------------------------

    m_horizontalScrollBar =
        new QScrollBar(
            Qt::Horizontal,
            this
        );

    const int scrollBarHeight =
        m_horizontalScrollBar->sizeHint().height();

    // --------------------------------------------------
    // Time ruler
    // --------------------------------------------------

    m_timeRuler =
        new TimeRuler(
            m_verticalScrollArea
        );

    m_timeRuler->setGeometry(
        m_leftContent->width(),
        scrollBarHeight,
        m_rightContent->width(),
        30
    );

    m_timeRuler->setFixedSize(
        m_rightContent->width(),
        30
    );

    m_timeRuler->show();
    m_timeRuler->raise();

    // --------------------------------------------------
    // Right container
    // --------------------------------------------------

    m_rightContainer =
        new QWidget;

    QVBoxLayout *rightContainerLayout =
        new QVBoxLayout(m_rightContainer);

    rightContainerLayout->setContentsMargins(
        0, 0, 0, 0
    );

    rightContainerLayout->setSpacing(0);

    m_horizontalScrollBar->setParent(
        m_rightContainer
    );

    rightContainerLayout->addWidget(
        m_horizontalScrollBar
    );

    m_horizontalScrollArea->setWidget(
        m_rightContent
    );

    rightContainerLayout->addWidget(
        m_horizontalScrollArea,
        1
    );
    // --------------------------------------------------
    // Assemble timeline
    // --------------------------------------------------

    timelineLayout->addWidget(
        m_leftContent
    );

    timelineLayout->addWidget(
        m_rightContainer,
        1
    );

    m_verticalScrollArea->setWidget(
        m_timelineContent
    );

    mainLayout->addWidget(
        m_verticalScrollArea
    );
    connect(
        m_horizontalScrollBar,
        &QScrollBar::valueChanged,
        this,
        [this](int value)
        {
            m_timeRuler->move(
                m_leftContent->width() - value,
                m_timeRuler->y()
            );
        }
    );

    connect(
        m_nativeHorizontalScrollBar,
        &QScrollBar::rangeChanged,
        m_horizontalScrollBar,
        &QScrollBar::setRange
    );

    connect(
        m_nativeHorizontalScrollBar,
        &QScrollBar::valueChanged,
        m_horizontalScrollBar,
        &QScrollBar::setValue
    );


    // --------------------------------------------------
    // Debug: outer vertical scrollbar
    // --------------------------------------------------

    connect(
        m_verticalScrollArea->verticalScrollBar(),
        &QScrollBar::valueChanged,
        this,
        [](int value)
        {
            qDebug()
                << "Timeline vertical:"
                << value;
        }
    );


}

void TimelineView::layoutSequences()
{
    const int scrollBarHeight =
        m_horizontalScrollArea
            ->horizontalScrollBar()
            ->sizeHint()
            .height();

    const int timeRulerHeight = 30;

    int leftY =
        scrollBarHeight + timeRulerHeight;

    int rightY =
        timeRulerHeight;

    for (int i = 0;
         i < m_sequences.size();
         ++i)
    {
        Sequence *sequence =
            m_sequences[i];

        int sequenceHeight =
            sequenceHeaderHeight;

        if (sequence->isExpanded())
        {
            sequenceHeight +=
                sequence->layers().size()
                * layerHeight;
        }

        m_leftSequenceContainers[i]->setGeometry(
            0,
            leftY,
            m_leftContent->width(),
            sequenceHeight
        );

        m_rightSequenceContainers[i]->setGeometry(
            0,
            rightY,
            m_rightContent->width(),
            sequenceHeight
        );

        leftY += sequenceHeight;
        rightY += sequenceHeight;
    }

    m_leftContent->resize(
        m_leftContent->width(),
        leftY
    );

    m_rightContent->resize(
        m_rightContent->width(),
        rightY
    );

    const int contentHeight =
        qMax(leftY, rightY);

    m_timelineContent->setMinimumHeight(
        contentHeight
    );
}

void TimelineView::addSequence(Sequence *sequence)
{
    // --------------------------------------------------
    // Store Sequence
    // --------------------------------------------------

    m_sequences.append(sequence);

    if (m_leftTopPadding == nullptr)
    {
        const int scrollBarHeight =
            m_horizontalScrollArea
                ->horizontalScrollBar()
                ->sizeHint()
                .height();

        const int timeRulerHeight = 30;

        m_leftTopPadding =
            new QWidget(m_leftContent);

        m_leftTopPadding->setFixedHeight(
            scrollBarHeight + timeRulerHeight
        );

        m_leftTopPadding->setStyleSheet(
            "background: transparent;"
        );

        m_leftTopPadding->show();
    }
    // --------------------------------------------------
    // LEFT container for this Sequence
    // --------------------------------------------------

    QWidget *leftSequenceContainer =
        new QWidget(m_leftContent);

    leftSequenceContainer->setObjectName(
        "leftSequenceContainer"
    );

    leftSequenceContainer->show();

    QVBoxLayout *leftSequenceLayout =
        new QVBoxLayout(leftSequenceContainer);

    leftSequenceLayout->setContentsMargins(0, 0, 0, 0);
    leftSequenceLayout->setSpacing(0);


    // --------------------------------------------------
    // RIGHT container for this Sequence
    // --------------------------------------------------

    QWidget *rightSequenceContainer =
        new QWidget(m_rightContent);

    rightSequenceContainer->setStyleSheet(
        "background: #040115;"
        "color: #ffffff;"
        "border: 1px solid #062003;"
    );

    rightSequenceContainer->show();

    QVBoxLayout *rightSequenceLayout =
        new QVBoxLayout(rightSequenceContainer);

    rightSequenceLayout->setContentsMargins(0, 0, 0, 0);
    rightSequenceLayout->setSpacing(0);


    // --------------------------------------------------
    // Store containers/layouts
    // --------------------------------------------------

    m_leftSequenceContainers.append(
        leftSequenceContainer
    );

    m_rightSequenceContainers.append(
        rightSequenceContainer
    );

    m_leftSequenceLayouts.append(
        leftSequenceLayout
    );

    m_rightSequenceLayouts.append(
        rightSequenceLayout
    );


    // --------------------------------------------------
    // Sequence header
    // --------------------------------------------------

    QWidget *header =
        sequence->headerWidget();

    header->setParent(leftSequenceContainer);

    leftSequenceLayout->addWidget(header);

    header->show();


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
    // before layerAdded() is connected.
    // --------------------------------------------------

    if (!sequence->layers().isEmpty())
    {
        Layer *layer =
            sequence->layers().first();

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
            Layer *layer =
                sequence->layers().last();

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


    // --------------------------------------------------
    // Expanded / collapsed
    // --------------------------------------------------

    connect(
        sequence,
        &Sequence::expandedChanged,
        this,
        [this, sequence](bool expanded)
        {
            for (Layer *layer : sequence->layers())
            {
                layer->leftWidget()->setVisible(
                    expanded
                );

                layer->clipArea()->setVisible(
                    expanded
                );
            }

            layoutSequences();
        }
    );


    // --------------------------------------------------
    // Calculate and position all Sequences
    // --------------------------------------------------

    layoutSequences();
}