#include "DrumSequence.h"
#include "../audio/AudioEngine.h"
#include "DrumGrid.h"
#include "StepGrid.h"
#include "drumPad.h"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QVBoxLayout>
DrumSequence::DrumSequence(int trackId,AudioEngine* audioEngine,QWidget* parent
)
    : QWidget(parent)
    , m_trackId(trackId)
    , m_audioEngine(audioEngine)
{
    createUi();
    createLayout();
    createConnections();
}

void DrumSequence::createUi()
{
    m_drumGrid = new DrumGrid;
    m_stepGrid = m_drumGrid->stepGrid();
    m_verticalScrollArea = new QScrollArea(this);

    m_verticalScrollArea->setWidget(m_drumGrid);
    m_verticalScrollArea->setWidgetResizable(false);

    m_verticalScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    m_verticalScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    m_drumPad = new DrumPad(this);
}

void DrumSequence::createLayout()
{
    auto* layout = new QHBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    layout->addWidget(
        m_verticalScrollArea,
        1
    );

    layout->addWidget(
        m_drumPad,
        0
    );
}

void DrumSequence::createConnections()
{
    connect(
        m_drumPad,
        &DrumPad::padPressed,
        this,
        [this](int padIndex)
        {
            qDebug() << "[DrumSequence]"
                    << "received padPressed:"
                    << padIndex;
            const DrumsetsSamples drum =
                m_drumPad->sampleForPad(padIndex);

            qDebug() << "[DrumSequence]"
                 << "drum:"
                 << static_cast<int>(drum);
            constexpr int velocity = 127;
            constexpr int midiNote = 60;
            if (!m_audioEngine)
            {
                qDebug() << "[DrumSequence]"
                        << "ERROR: audioEngine is null";
                return;
            }
            qDebug() << "[DrumSequence]"
                 << "calling drumNoteOn:"
                 << "track =" << m_trackId
                 << "note =" << midiNote
                 << "velocity =" << velocity;
            if (m_audioEngine)
            {
                m_audioEngine->drumNoteOn(
                    m_trackId,
                    drum,
                    midiNote,
                    velocity
                );
            }
        }
    );
    connect(
        m_stepGrid,
        &StepGrid::patternChanged,
        this,
        [this]()
        {
            emit patternChanged();
        }
    );

    connect(
        m_stepGrid,
        &StepGrid::cellClicked,
        this,
        [this](int row, int step)
        {
            const DrumsetsSamples drum =
                m_drumPad->sampleForPad(row);

            if (drum == DrumsetsSamples::None)
                return;

            int existingIndex = -1;

            for (int i = 0; i < m_drumSteps.size(); ++i)
            {
                const DrumStepEvent& event =
                    m_drumSteps.at(i);

                if (event.drum == drum &&
                    event.step == step)
                {
                    existingIndex = i;
                    break;
                }
            }

            if (existingIndex >= 0)
            {
                m_drumSteps.removeAt(existingIndex);
            }
            else
            {
                DrumStepEvent event;

                event.drum = drum;
                event.step = step;
                event.velocity = 127;

                m_drumSteps.append(event);
            }

            qDebug()
                << "[DrumSequence]"
                << "pattern changed:"
                << "drum =" << static_cast<int>(drum)
                << "step =" << step
                << "events =" << m_drumSteps.size();
            emit drumPatternChanged(m_drumSteps);
        }
    );
}

const QVector<DrumStepEvent> &DrumSequence::drumSteps() const
{
    return m_drumSteps;
}