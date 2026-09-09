#include <QApplication>

#include "base.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Base window;
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
