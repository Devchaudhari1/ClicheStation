#pragma once

#include <QWidget>

class RowHeader : public QWidget
{
    Q_OBJECT

public:
    explicit RowHeader(QWidget* parent = nullptr);
    ~RowHeader() override = default;

    void setRowCount(int count);

    int rowCount() const;

    QSize sizeHint() const override;

signals:
    void rowClicked(int row);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    int rowHeight() const;

private:
    int m_rowCount = 25;
    int m_rowHeight = 32;
    int m_width = 200;
};