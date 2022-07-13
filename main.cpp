#include "mainplot.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainPlot w;
    w.show();
    return a.exec();
}
