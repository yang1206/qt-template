#include <QApplication>
#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    MainWindow w;
    w.show();
    return a.exec();
}
