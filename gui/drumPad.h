#ifndef DRUMPAD_H
#define DRUMPAD_H
#include <QWidget>
class DrumPad : public QWidget
{
    public:
    explicit DrumPad(QWidget *parent = nullptr);
    ~DrumPad();
    void createPad();
};

#endif // DRUMPAD_H