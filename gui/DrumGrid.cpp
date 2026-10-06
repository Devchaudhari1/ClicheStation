#include "DrumGrid.h"
#include "RowContainer.h"
#include <QVBoxLayout>

DrumGrid::DrumGrid(QWidget* parent)
    : QWidget(parent)
{
    createUi();
    createLayout();
}

void DrumGrid::createUi()
{
    m_rowContainer = new RowContainer(this);
}

void DrumGrid::createLayout()
{
    auto* layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(m_rowContainer);
}

QSize DrumGrid::sizeHint() const
{
    if (!m_rowContainer)
        return QWidget::sizeHint();

    return QSize(QWidget::sizeHint().width(),m_rowContainer->sizeHint().height());
}

StepGrid* DrumGrid::stepGrid() const
{
    return m_rowContainer->stepGrid();
}