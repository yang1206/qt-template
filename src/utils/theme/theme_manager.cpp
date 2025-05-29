#include "theme_manager.h"
#include <QApplication>
#include <QStyleHints>

ThemeManager &ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager() : QObject(nullptr), m_currentTheme(Type::System), m_isDarkMode(false) {

    // 获取初始系统主题
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    bool initialIsDark = qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    const QPalette defaultPalette = qApp->palette();
    bool initialIsDark =
            defaultPalette.color(QPalette::WindowText).lightness() > defaultPalette.color(QPalette::Window).lightness();
#endif
    // 设置初始暗色模式状态
    m_isDarkMode = initialIsDark;
    applyTheme(m_isDarkMode);


    // 监听系统主题变化
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme scheme) {
        if (m_currentTheme == Type::System) {
            applyTheme(scheme == Qt::ColorScheme::Dark);
        }
    });
#else
    connect(qApp, &QApplication::paletteChanged, this, [this]() {
        if (m_currentTheme == Type::System) {
            updateSystemTheme();
        }
    });
#endif
}

void ThemeManager::updateSystemTheme() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    bool isDark = qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    const QPalette defaultPalette;
    bool isDark =
            defaultPalette.color(QPalette::WindowText).lightness() > defaultPalette.color(QPalette::Window).lightness();
#endif


    applyTheme(isDark);
}

void ThemeManager::setTheme(Type theme) {
    if (m_currentTheme == theme)
        return;

    m_currentTheme = theme;

    if (theme == Type::System) {
        updateSystemTheme();
    } else {
        applyTheme(theme == Type::Dark);
    }
}

void ThemeManager::applyTheme(bool isDark) {
    m_isDarkMode = isDark;
    emit themeChanged();
}
