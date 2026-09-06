#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class Layer;
class TimelineRow;

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

    QWidget *layerContainer() const;
    QWidget *headerWidget() const;

    QVector<TimelineRow *> layerRows() const;
    void detachHeader();

signals:
    void layerAdded();
    void expandedChanged(bool expanded);

private:
    void toggleExpanded();

    int m_sequenceId;
    QWidget *m_headerSpacer;
    QWidget *m_header;
    QPushButton *m_expandButton;
    QLabel *m_sequenceLabel;
    QPushButton *m_addLayerButton;

    QWidget *m_layerContainer;
    QVBoxLayout *m_layerLayout;

    QVector<QPair<int, QString>> m_availableTracks;

    bool m_expanded = true;
};