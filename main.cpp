#include "mainwindow.h"

#include <QApplication>

#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client c;
    c.show();
//    MainWindow w;
//    w.show();
    return a.exec();
}
