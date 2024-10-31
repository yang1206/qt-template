#include <QApplication>
#include "mainwindow.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    int fontId = QFontDatabase::addApplicationFont(":/include/Font/ElaAwesome.ttf");
    if (fontId == -1) {
        qDebug() << "Failed to load font ElaAwesome.ttf";
    } else {
        qDebug() << "Successfully loaded font, id:" << fontId;
        qDebug() << "Font families:" << QFontDatabase::applicationFontFamilies(fontId);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
