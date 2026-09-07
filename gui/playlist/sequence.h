#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>

class QPushButton;
class QLabel;
class Layer;

class Sequence : public QWidget
{
    Q_OBJECT

public:
    explicit Sequence(
        int sequenceId,
        QWidget *parent = nullptr
    );

    int sequenceId() const;

    Layer *addLayer();

    void setAvailableTracks(
        const QVector<QPair<int, QString>> &tracks
    );

    QWidget *headerWidget() const;
    bool isExpanded() const;
    const QVector<Layer *> &layers() const;

signals:
    void layerAdded();
    void expandedChanged(bool expanded);

private:
    void toggleExpanded();

    int m_sequenceId;

    QWidget *m_header;
    QPushButton *m_expandButton;
    QLabel *m_sequenceLabel;
    QPushButton *m_addLayerButton;

    QVector<Layer *> m_layers;

    QVector<QPair<int, QString>> m_availableTracks;

    bool m_expanded = true;
};