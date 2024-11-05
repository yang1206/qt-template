#include <QApplication>
#include <QScreen>
#include "ElaApplication.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    eApp->init();
    MainWindow w;
    w.show();
    return a.exec();
}
