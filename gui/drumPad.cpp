#include "DrumPad.h"
#include "../synth/DrumsetsSamples.h"
#include "../audio/AudioEngine.h"
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QVector>
#include <QColor>
#include <QMouseEvent>

DrumPad::DrumPad(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Drum Pad");
    setFixedSize(500,500);

    setStyleSheet(
        "DrumPad {"
        "    background-color: #151515;"
        "}"
    );

    createPad();
}

DrumPad::~DrumPad()
{
}

DrumsetsSamples DrumPad::sampleForPad(int padIndex) const
{
    static const QVector<DrumsetsSamples> padSamples =
    {
        DrumsetsSamples::kick,
        DrumsetsSamples::snare,
        DrumsetsSamples::hh_closed,
        DrumsetsSamples::hh_open,
        DrumsetsSamples::hand_clap,

        DrumsetsSamples::tom_1,
        DrumsetsSamples::tom_2,
        DrumsetsSamples::tom_3,
        DrumsetsSamples::crash_1,
        DrumsetsSamples::crash_2,

        DrumsetsSamples::ride,
        DrumsetsSamples::ride_bell,
        DrumsetsSamples::china,
        DrumsetsSamples::splash,
        DrumsetsSamples::stick_click,

        DrumsetsSamples::snare_rim,
        DrumsetsSamples::snare_sidestick,
        DrumsetsSamples::hh_foot,
        DrumsetsSamples::crash_3,
        DrumsetsSamples::tom_1_rim,

        DrumsetsSamples::tom_2_rim,
        DrumsetsSamples::tom_3_rim,
        DrumsetsSamples::snare_snares_off,
        DrumsetsSamples::snare_snares_off_rim,
        DrumsetsSamples::snare_snares_off_sidestick
    };

    if (padIndex < 0 || padIndex >= padSamples.size())
        return DrumsetsSamples::kick;

    return padSamples[padIndex];
}

void DrumPad::createPad()
{
    qDebug() << "kick exists:"
             << QFile(":/drums/kick.png").exists();

    qDebug() << "icon exists:"
             << QFile(":/icons/cliche_station.png").exists();

    qDebug() << "root resources:"
             << QDir(":/").entryList();

    qDebug() << "drum resources:"
             << QDir(":/drums").entryList();


    QGridLayout *padLayout = new QGridLayout(this);

    padLayout->setSpacing(10);
    padLayout->setContentsMargins(12, 12, 12, 12);

    /*
     * One color for each pad.
     *
     * The colors are intentionally fairly bright because
     * the black silhouettes will be displayed on top.
     */
    const QVector<QColor> padColors =
    {
        QColor("#ff4d6d"), // kick
        QColor("#ff7b00"), // snare
        QColor("#ffd000"), // hh closed
        QColor("#a8ff00"), // hh open
        QColor("#00e676"), // hand clap

        QColor("#00d9ff"), // tom 1
        QColor("#00a8ff"), // tom 2
        QColor("#0066ff"), // tom 3
        QColor("#654cff"), // crash 1
        QColor("#9b4dff"), // crash 2

        QColor("#d94dff"), // ride
        QColor("#ff4dc4"), // ride bell
        QColor("#ff4d91"), // china
        QColor("#ff7043"), // splash
        QColor("#ffab40"), // stick click

        QColor("#c6ff00"), // snare rim
        QColor("#69f0ae"), // sidestick
        QColor("#18ffff"), // hh foot
        QColor("#40c4ff"), // crash 3
        QColor("#7c4dff"), // tom 1 rim

        QColor("#e040fb"), // tom 2 rim
        QColor("#ff4081"), // tom 3 rim
        QColor("#ff5252"), // snares off
        QColor("#ff9100"), // snares off rim
        QColor("#76ff03")  // snares off sidestick
    };

    for (int row = 0; row < 5; ++row)
    {
        for (int col = 0; col < 5; ++col)
        {
            const int padIndex = row * 5 + col;

            const DrumsetsSamples sample =
                sampleForPad(padIndex);

            QPushButton *padButton =
                new QPushButton(this);

            padButton->setFixedSize(82, 82);

            /*
             * -------------------------------------------------
             * COLOR
             * -------------------------------------------------
             */

            const QColor color = padColors[padIndex];

            const QString background =
                color.darker(450).name();

            const QString backgroundHover =
                color.darker(300).name();

            const QString backgroundPressed =
                color.darker(180).name();

            const QString border =
                color.name();

            padButton->setStyleSheet(
                QString(
                    "QPushButton {"
                    "    background-color: %1;"
                    "    border: 4px solid %2;"
                    "    border-radius: 14px;"
                    "    padding: 4px;"
                    "}"
                    ""
                    "QPushButton:hover {"
                    "    background-color: %3;"
                    "    border: 4px solid %2;"
                    "}"
                    ""
                    "QPushButton:pressed {"
                    "    background-color: %4;"
                    "    border: 4px solid white;"
                    "}"
                )
                .arg(background)
                .arg(border)
                .arg(backgroundHover)
                .arg(backgroundPressed)
            );

            /*
             * -------------------------------------------------
             * DRUM SILHOUETTE
             * -------------------------------------------------
             */

            const QString imagePath =
                ":/assets/drums/" +
                drumSampleName(sample) +
                ".png";
            qDebug() << "Trying drum image:" << imagePath;
            QPixmap pixmap(imagePath);

            if (!pixmap.isNull())
            {
                /*
                 * Scale the image while preserving its aspect ratio.
                 */
                qDebug()
                    << "LOADED:"
                    << imagePath
                    << "size:"
                    << pixmap.size();
                pixmap = pixmap.scaled(
                    58,
                    58,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                );

                /*
                 * Make the visible pixels BLACK while preserving
                 * the alpha/transparency of the original PNG.
                 *
                 * This is especially useful if your PNG contains
                 * a colored/grey silhouette.
                 */
                QImage image = pixmap.toImage()
                                   .convertToFormat(QImage::Format_ARGB32);

                for (int y = 0; y < image.height(); ++y)
                {
                    for (int x = 0; x < image.width(); ++x)
                    {
                        QColor pixel = image.pixelColor(x, y);

                        if (pixel.alpha() > 0)
                        {
                            pixel.setRed(0);
                            pixel.setGreen(0);
                            pixel.setBlue(0);

                            image.setPixelColor(x, y, pixel);
                        }
                    }
                }

                QLabel *imageLabel =
                    new QLabel(padButton);

                imageLabel->setPixmap(QPixmap::fromImage(image));

                imageLabel->setAlignment(Qt::AlignCenter);

                /*
                 * Don't let the label consume mouse clicks.
                 */
                imageLabel->setAttribute(
                    Qt::WA_TransparentForMouseEvents
                );

                /*
                 * Make the image fill the pad.
                 */
                imageLabel->setGeometry(
                    8,
                    8,
                    padButton->width() - 16,
                    padButton->height() - 16
                );

                imageLabel->raise();
            }
            else
            {
                qDebug()
                    << "Could not load drum image:"
                    << imagePath;
            }

            padLayout->addWidget(
                padButton,
                row,
                col
            );

            connect(
                padButton,
                &QPushButton::pressed,
                this,
                [this, padIndex]()
                {
                    qDebug() << "[DrumPad]"
                    << "padPressed:"
                    << padIndex;
                    emit padPressed(padIndex);
                }
            );
        }
    }
}