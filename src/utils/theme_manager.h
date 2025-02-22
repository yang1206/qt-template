#pragma once
#include <QObject>

#define eThemeManager ThemeManager::instance()
#define isDarkTheme eThemeManager.isDarkMode()
class ThemeManager : public QObject {
    Q_OBJECT

public:
    static ThemeManager &instance();

    // 主题切换
    enum class Type { Light, Dark, System };
    void setTheme(Type theme);
    Type currentTheme() const { return m_currentTheme; }
    bool isDarkMode() const { return m_isDarkMode; }
    void notifyThemeChange() { emit themeChanged(); }

signals:
    void themeChanged();

private:
    ThemeManager();
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager &) = delete;
    ThemeManager &operator=(const ThemeManager &) = delete;

    Type m_currentTheme;
    bool m_isDarkMode;

    void updateSystemTheme();
    void applyTheme(bool isDark);
};
