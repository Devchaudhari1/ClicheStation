#include "TimelineView.h"

#include <QScrollArea>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>


TimelineView::TimelineView(QWidget *parent)
    : QWidget(parent),
      m_leftPane(nullptr),
      m_rightPane(nullptr),
      m_leftContent(nullptr),
      m_rightContent(nullptr)
{
    // ----------------------------------------
    // TimelineView
    // ----------------------------------------

    QHBoxLayout *mainLayout =
        new QHBoxLayout(this);

    mainLayout->setContentsMargins(
        0, 0, 0, 0
    );

    mainLayout->setSpacing(0);

    // ----------------------------------------
    // Left Pane
    // ----------------------------------------

    m_leftPane =
        new QScrollArea(this);

    m_leftPane->setFixedWidth(160);

    m_leftPane->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_leftPane->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_leftPane->setWidgetResizable(false);

    m_leftContent =
        new QWidget;

    m_leftContent->setStyleSheet(
        "background: darkblue;"
    );

    QVBoxLayout *leftLayout =
        new QVBoxLayout(m_leftContent);

    leftLayout->setContentsMargins(
        0, 0, 0, 0
    );

    leftLayout->setSpacing(0);

    // ----------------------------------------
    // Right Pane
    // ----------------------------------------

    m_rightPane =
        new QScrollArea(this);

    m_rightPane->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    m_rightPane->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_rightPane->setWidgetResizable(false);

    m_rightContent =
        new QWidget;

    m_rightContent->setStyleSheet(
        "background: darkgreen;"
    );

    QVBoxLayout *rightLayout =
        new QVBoxLayout(m_rightContent);

    rightLayout->setContentsMargins(
        0, 0, 0, 0
    );

    rightLayout->setSpacing(0);

    // ----------------------------------------
    // Temporary test rows
    // ----------------------------------------

    for (int sequence = 1;
         sequence <= 3;
         ++sequence)
    {
        QWidget *leftSequence =
            new QLabel(
                QString("Sequence %1").arg(sequence),
                m_leftContent
            );

        leftSequence->setFixedHeight(40);
        leftSequence->setStyleSheet(
            "background: orange;"
            "border: 1px solid black;"
        );

        leftLayout->addWidget(
            leftSequence
        );


        QWidget *rightSequence =
            new QLabel(
                QString("Sequence %1").arg(sequence),
                m_rightContent
            );

        rightSequence->setFixedHeight(40);
        rightSequence->setStyleSheet(
            "background: orange;"
            "border: 1px solid black;"
        );

        rightLayout->addWidget(
            rightSequence
        );


        int layerCount = sequence + 2;

        for (int layer = 1;
             layer <= layerCount;
             ++layer)
        {
            QWidget *leftLayer =
                new QLabel(
                    QString("Layer %1").arg(layer),
                    m_leftContent
                );

            leftLayer->setFixedHeight(70);
            leftLayer->setStyleSheet(
                "background: steelblue;"
                "border: 1px solid black;"
            );

            leftLayout->addWidget(
                leftLayer
            );


            QWidget *rightLayer =
                new QLabel(
                    QString("Layer %1 ClipArea").arg(layer),
                    m_rightContent
                );

            rightLayer->setFixedHeight(70);
            rightLayer->setMinimumWidth(3000);

            rightLayer->setStyleSheet(
                "background: seagreen;"
                "border: 1px solid black;"
            );

            rightLayout->addWidget(
                rightLayer
            );
        }
    }

    // ----------------------------------------
    // Install content
    // ----------------------------------------

    m_leftPane->setWidget(
        m_leftContent
    );

    m_rightPane->setWidget(
        m_rightContent
    );

    // ----------------------------------------
    // Add panes
    // ----------------------------------------

    mainLayout->addWidget(
        m_leftPane
    );

    mainLayout->addWidget(
        m_rightPane,
        1
    );
}