#include <QApplication>
#include <QFile>
#include "mainwindow.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    mainwindow w;
    QStringList styleFiles = {":/styles/pushbutton.qss", ":/styles/combox.qss", ":/styles/spinbox.qss"};
    QString combinedStyleSheet;

    for (const QString &styleFile: styleFiles) {
        QFile file(styleFile);
        if (file.open(QFile::ReadOnly)) {
            combinedStyleSheet += QLatin1String(file.readAll());
            file.close();
        }
    }

    // 设置合并后的样式表
    a.setStyleSheet(combinedStyleSheet);
    w.show();
    return a.exec();
}
