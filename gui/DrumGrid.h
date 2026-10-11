#pragma once

#include <QWidget>
#include <QSize>
class RowContainer;
class StepGrid;
class DrumGrid : public QWidget
{
    Q_OBJECT

public:
    explicit DrumGrid(QWidget* parent = nullptr);
    ~DrumGrid() override = default;
    StepGrid* stepGrid() const;
    QSize sizeHint() const override;

private:
    void createUi();
    void createLayout();

private:
    RowContainer* m_rowContainer = nullptr;
};