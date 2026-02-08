#include "QtNativeUI/NConfig.h"
#include "mainwindow.h"

void setupStyle() {
    // 设置默认样式
    QFile styleFile(":/style/reset.qss");
    styleFile.open(QFile::ReadOnly);
    qApp->setStyleSheet(styleFile.readAll());
    QFont font = qApp->font();
    font.setPixelSize(13);
#ifdef Q_OS_WINDOWS
    font.setFamily("Microsoft YaHei");
#else
    font.setFamily("Noto Sans CJK SC");
#endif
    font.setHintingPreference(QFont::PreferNoHinting);
    qApp->setFont(font);
    styleFile.close();
}


int main(int argc, char* argv[]) {
    QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    nConfig->initialize();
    setupStyle();

    MainWindow w;
    w.show();
    return a.exec();
}
