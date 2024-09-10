#include <QApplication>
#include <QFile>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    mainwindow w;
    QFile pushButtonStyle(":/styles/pushbutton.qss");
    if (pushButtonStyle.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(pushButtonStyle.readAll());
        a.setStyleSheet(styleSheet);
    }

    QFile comboBoxStyle(":/styles/combox.qss");
    if (comboBoxStyle.open(QFile::ReadOnly)) {
        QString styleSheet1 = QLatin1String(comboBoxStyle.readAll());
        a.setStyleSheet(styleSheet1);
    }


    pushButtonStyle.close();
    comboBoxStyle.close();
    w.show();
    return a.exec();
}
