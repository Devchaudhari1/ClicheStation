#include "RowContainer.h"
#include "RowHeader.h"
#include "StepGrid.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>

RowContainer::RowContainer(QWidget* parent)
    : QWidget(parent)
{
    createUi();
    createLayout();
}

void RowContainer::createUi()
{
    m_rowHeader = new RowHeader(this);

    m_stepGrid = new StepGrid;

    m_horizontalScrollArea =
        new QScrollArea(this);

    m_horizontalScrollArea->setWidget(
        m_stepGrid
    );

    m_horizontalScrollArea->setWidgetResizable(false);

    m_horizontalScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_horizontalScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_horizontalScrollArea->setFrameShape(
        QFrame::NoFrame
    );
}

StepGrid* RowContainer::stepGrid() const
{
    return m_stepGrid;
}

void RowContainer::createLayout()
{
    auto* horizontalScrollBar =
        m_horizontalScrollArea->horizontalScrollBar();

    // The QScrollArea no longer displays its scrollbar itself.
    // We use the same scrollbar above the viewport.
    horizontalScrollBar->setParent(this);

    // -------------------------
    // Left: RowHeader
    // -------------------------

    auto* headerLayout = new QVBoxLayout;

    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(0);

    // Padding matching the horizontal scrollbar height.
    auto* headerPadding = new QWidget;

    headerPadding->setFixedHeight(
        horizontalScrollBar->sizeHint().height()
    );

    headerLayout->addWidget(
        headerPadding
    );

    headerLayout->addWidget(
        m_rowHeader,
        1
    );

    // -------------------------
    // Right: ScrollBar + Grid
    // -------------------------

    auto* gridLayout = new QVBoxLayout;

    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);

    gridLayout->addWidget(
        horizontalScrollBar
    );

    gridLayout->addWidget(
        m_horizontalScrollArea,
        1
    );

    // -------------------------
    // RowContainer
    // -------------------------

    auto* layout = new QHBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addLayout(
        headerLayout
    );

    layout->addLayout(
        gridLayout,
        1
    );
}