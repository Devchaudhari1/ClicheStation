#include <QApplication>
#include <QFile>
#include <QDir>
#include "base.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/cliche_station.png"));
    Base window;
    qDebug() << "kick:"
            << QFile(":/assets/drums/kick.png").exists();

    qDebug() << "snare:"
            << QFile(":/assets/drums/snare.png").exists();

    qDebug() << "icon:"
            << QFile(":/assets/icons/cliche_station.png").exists();

    qDebug() << "assets:"
            << QDir(":/assets").entryList();

    qDebug() << "drums:"
            << QDir(":/assets/drums").entryList();
    app.setStyleSheet(R"(
jj
        QWidget {
            background-color: #303030;
        }

        QPushButton#whiteKey {
            background-color: white;
            color: black;

            border: 1px solid #222222;

            border-bottom-left-radius: 4px;
            border-bottom-right-radius: 4px;

            font-size: 11px;

            padding-top: 170px;
        }

        QPushButton#whiteKey:hover {
            background-color: #f2f2f2;
        }

        QPushButton#whiteKey:pressed {
            background-color: #cccccc;
        }


        QPushButton#blackKey {
            background-color: #111111;
            color: white;

            border: 1px solid black;

            border-bottom-left-radius: 5px;
            border-bottom-right-radius: 5px;

            font-size: 9px;

            padding-top: 90px;
        }

        QPushButton#blackKey:hover {
            background-color: #252525;
        }

        QPushButton#blackKey:pressed {
            background-color: #555555;
        }

    )");
    window.show();

    return app.exec();
}
