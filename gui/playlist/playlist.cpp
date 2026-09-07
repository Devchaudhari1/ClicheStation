#include "Playlist.h"
#include "TimeRuler.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>
#include <QLabel>


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

    m_leftPane->setFixedWidth(160);

    m_leftPane->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_leftPane->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_leftPane->setWidgetResizable(false);


    // ---------------------------------------------------------
    // Left Content
    // ---------------------------------------------------------

    m_leftContent = new QWidget;

    m_leftContent->setStyleSheet(
        "background: darkblue;"
    );

    m_leftLayout = new QVBoxLayout(m_leftContent);

    m_leftLayout->setContentsMargins(0, 0, 0, 0);
    m_leftLayout->setSpacing(0);


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

    m_rightContent->setStyleSheet(
        "background: darkgreen;"
    );

    m_rightLayout = new QVBoxLayout(m_rightContent);

    m_rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightLayout->setSpacing(0);


    // =========================================================
    // Install Content
    // =========================================================

    m_leftPane->setWidget(m_leftContent);
    m_rightPane->setWidget(m_rightContent);


    // =========================================================
    // Timeline Layout
    // =========================================================

    timelineLayout->addWidget(m_leftPane);
    timelineLayout->addWidget(m_rightPane, 1);


    // =========================================================
    // Add Timeline View to Playlist
    // =========================================================

    playlistLayout->addWidget(m_timelineView, 1);

     timelineLayout->addWidget(m_leftPane);
    timelineLayout->addWidget(m_rightPane, 1);

    playlistLayout->addWidget(m_timelineView, 1);
    // =========================================================
    // TEMPORARY TEST CONTENT
    // =========================================================

    const int sequenceHeight = 40;
    const int layerHeight = 70;

    for (int sequence = 1; sequence <= 3; ++sequence)
    {
        // Sequence header - left
        m_leftLayout->addWidget(
            makeTestWidget(
                QString("Sequence %1").arg(sequence),
                sequenceHeight,
                "orange",
                m_leftContent
            )
        );

        // Sequence header - right
        m_rightLayout->addWidget(
            makeTestWidget(
                QString("Sequence %1").arg(sequence),
                sequenceHeight,
                "orange",
                m_rightContent
            )
        );

        int layerCount = sequence + 2;

        for (int layer = 1; layer <= layerCount; ++layer)
        {
            // Layer - left
            m_leftLayout->addWidget(
                makeTestWidget(
                    QString("Layer %1").arg(layer),
                    layerHeight,
                    "steelblue",
                    m_leftContent
                )
            );

            // ClipArea - right
            m_rightLayout->addWidget(
                makeTestWidget(
                    QString("Layer %1 ClipArea").arg(layer),
                    layerHeight,
                    "seagreen",
                    m_rightContent
                )
            );
        }
    }
    m_rightContent->setMinimumWidth(3000);

    m_leftContent->adjustSize();
    m_rightContent->adjustSize();

}