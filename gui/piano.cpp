#include "piano.h"
#include "pianoRoll.h"
#include "pianoKeyboard.h"
#include "../audio/AudioEngine.h"
#include <QScrollArea>
#include <QVBoxLayout>

Piano::Piano(
    int trackId,
    AudioEngine *audioEngine,
    QWidget *parent
)
    : QWidget(parent, Qt::Window),
      trackId(trackId),
      pianoRoll(nullptr),
      pianoKeyboard(nullptr),
      audioEngine(audioEngine)
{
    setWindowTitle("88 Key Piano");
    resize(1400, 700);

    createUI();
}

Piano::~Piano()
{
}

const QVector<PlacedNote>& Piano::notes() const
{
    return pianoRoll->notes();
}

void Piano::createUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Piano roll
    QScrollArea *scrollArea =
    new QScrollArea(this);

    pianoRoll = new PianoRoll;

    scrollArea->setWidget(pianoRoll);

    scrollArea->setWidgetResizable(false);

    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    // Physical/visual piano keyboard
    pianoKeyboard = new PianoKeyboard(this);
    pianoKeyboard->setFixedHeight(180);

    mainLayout->addWidget(scrollArea, 1);
    mainLayout->addWidget(pianoKeyboard);
    connect(
    pianoKeyboard,
    &PianoKeyboard::notePressed,
    this,
    [this](int midiNote)
    {
        if(audioEngine)
        {
            audioEngine->noteOn(trackId, midiNote, 100);
        }
    }
    );

    connect(
    pianoKeyboard,
    &PianoKeyboard::noteReleased,
    this,
    [this](int midiNote)
    {
        if(audioEngine)
        {
            audioEngine->noteOff(trackId,midiNote);
        }
    }
    );
    
    connect(
        pianoRoll,
        &PianoRoll::becameActive,
        this,
        [this](PianoRoll* pianoRoll)
        {
            emit pianoRollBecameActive(pianoRoll);
        }
    );
    /*
     * Later we can connect the keyboard to other systems here.
     *
     * For example:
     *
     * connect(
     *     pianoKeyboard,
     *     &PianoKeyboard::notePressed,
     *     this,
     *     ...
     * );
     */
}