#pragma once

#include <QWidget>
#include <QSet>

class StepGrid : public QWidget
{
    Q_OBJECT

public:
    explicit StepGrid(QWidget* parent = nullptr);
    ~StepGrid() override = default;

    void setRowCount(int count);
    void setStepCount(int count);

    int rowCount() const;
    int stepCount() const;

    QSize sizeHint() const override;

signals:
    void cellClicked(int row, int step);
    void patternChanged();
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    int cellWidth() const;
    int cellHeight() const;

    bool isCellActive(int row, int step) const;
    void setCellActive(int row, int step, bool active);

private:
    int m_rowCount = 25;
    int m_stepCount = 30;

    int m_cellWidth = 48;
    int m_cellHeight = 32;

    QSet<quint64> m_activeCells;
};