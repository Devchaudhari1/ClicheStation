#include "piano.h"
#include "pianoRoll.h"
#include "pianoKeyboard.h"
#include "../audio/AudioEngine.h"

#include <QVBoxLayout>

Piano::Piano(AudioEngine *audioEngine, QWidget *parent)
    : QWidget(parent, Qt::Window),
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

void Piano::createUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Piano roll
    pianoRoll = new PianoRoll(this);

    // Physical/visual piano keyboard
    pianoKeyboard = new PianoKeyboard(this);
    pianoKeyboard->setFixedHeight(180);

    mainLayout->addWidget(pianoRoll, 1);
    mainLayout->addWidget(pianoKeyboard);
    connect(
    pianoKeyboard,
    &PianoKeyboard::notePressed,
    this,
    [this](int midiNote)
    {
        audioEngine->noteOn(midiNote, 100);
    }
    );

    connect(
    pianoKeyboard,
    &PianoKeyboard::noteReleased,
    this,
    [this](int midiNote)
    {
        audioEngine->noteOff(midiNote);
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