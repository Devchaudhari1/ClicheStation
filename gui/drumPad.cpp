#include "DrumPad.h"

#include <QPushButton>
#include <QGridLayout>

DrumPad::DrumPad(QWidget * parent)
    :QWidget(parent)
{
   setWindowTitle("Drum Pad");
   resize(500, 500);
   createPad();
}

DrumPad::~DrumPad()
{

}

void DrumPad::createPad()
{
    QWidget * padWindow = new QWidget(this);
    QGridLayout *padLayout = new QGridLayout(padWindow);
    for (int row = 0; row < 5; ++row)
    {
        for (int col = 0; col < 5; ++col)
        {
            QPushButton * padButton = new QPushButton(padWindow);
            padButton->setFixedSize(80, 80);
            padButton->setStyleSheet("background-color: #444444; border-radius: 10px;");
            padLayout->addWidget(padButton, row, col);

            connect(padButton, &QPushButton::pressed, this, [row, col]()
            {
                qDebug() << "Pad pressed at (" << row << "," << col << ")";
            });
        }
    }
}