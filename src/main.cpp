#include <QApplication>
#include <QScreen>
#include "mainwindow.h"
#include "utils/theme/theme_manager.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    ThemeManager::instance();
    MainWindow w;
    w.show();
    return a.exec();
}
