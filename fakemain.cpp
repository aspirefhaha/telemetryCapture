#include "telefakewin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TeleFakeWin w;
    w.show();

    return a.exec();
}
