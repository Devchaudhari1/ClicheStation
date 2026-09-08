#pragma once

#include <QWidget>
#include <QVector>

class ClipVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit ClipVisualizer(QWidget *parent = nullptr);

    void setSamples(const QVector<float> &samples);
    void setDuration(double duration);

    double duration() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> m_samples;
    double m_duration = 0.0;
};