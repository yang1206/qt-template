#include <QApplication>
#include <QScreen>
#include "ElaApplication.h"
#include "mainwindow.h"
#include "utils/theme_manager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    eApp->init();
    ThemeManager::instance();
    MainWindow w;
    w.show();

#ifdef NDEBUG
    qDebug() << "Release Build";
#else
    qDebug() << "Debug Build";
#endif

#ifdef QT_DEBUG
    qDebug() << "Qt Debug Mode";
#else
    qDebug() << "Qt Release Mode";
#endif
    return a.exec();
}
