#pragma once

#include <QWidget>
#include <QResizeEvent>
#include <QSize>
class RowHeader;
class QScrollArea;
class StepGrid;

class RowContainer : public QWidget
{
    Q_OBJECT

public:
    explicit RowContainer(QWidget* parent = nullptr);
    ~RowContainer() override = default;
    StepGrid* stepGrid() const;

private:
    void createUi();
    void createLayout();
    RowHeader* m_rowHeader = nullptr;
    QScrollArea* m_horizontalScrollArea = nullptr;
    StepGrid* m_stepGrid = nullptr;
};