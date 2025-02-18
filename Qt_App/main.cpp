#include "weatherviewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WeatherViewer w;
    w.show();
    return a.exec();
}
