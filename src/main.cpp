#include <QApplication>
#include <QScreen>
#include "ElaApplication.h"
#include "mainwindow.h"
#include "utils/theme/theme_manager.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    eApp->init();
    ThemeManager::instance();
    MainWindow w;
    w.show();
    return a.exec();
}
