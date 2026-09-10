#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>

class QPushButton;
class QLabel;
class Layer;
class VoiceTrack;
class TrackList;
class AudioEngine;

class Sequence : public QWidget
{
    Q_OBJECT

public:
    explicit Sequence(
        int sequenceId,
        AudioEngine *m_audioEngine,
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
    void setTrackList(TrackList *trackList);

signals:
    void layerAdded();
    void expandedChanged(bool expanded);

private:
    void toggleExpanded();

    int m_sequenceId;

    AudioEngine *m_audioEngine;
    
    QWidget *m_header;
    QPushButton *m_expandButton;
    QLabel *m_sequenceLabel;
    QPushButton *m_addLayerButton;

    QVector<Layer *> m_layers;

    QVector<VoiceTrack*> m_availableVoiceTracks;
    TrackList *m_trackList = nullptr;
    QVector<QPair<int, QString>> m_availableTracks;

    bool m_expanded = true;
};