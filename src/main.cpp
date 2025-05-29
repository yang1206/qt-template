#include "mainwindow.h"
#include "utils/theme/theme_manager.h"

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
    QApplication a(argc, argv);
    setupStyle();
    ThemeManager::instance();
    MainWindow w;
    w.show();
    return a.exec();
}
