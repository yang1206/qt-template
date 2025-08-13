#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QObject>
#include <QWKWidgets/widgetwindowagent.h>
#include <QWidget>

#include "utils/theme/theme_manager.h"
#include "windowbar.h"
#include "windowbutton.h"

class WindowFrameManager : public QObject {
    Q_OBJECT

public:
    explicit WindowFrameManager(QMainWindow* window);
    ~WindowFrameManager();

    // 初始化窗口框架和按钮
    void setupWindowFrame();
    
    // 加载主题样式
    void loadTheme(ThemeManager::Type theme);
    
    // 获取窗口代理
    QWK::WidgetWindowAgent* windowAgent() const { return m_windowAgent; }
    
    // 获取菜单栏
    QMenuBar* menuBar() const { return m_menuBar; }
    
    // 获取窗口栏
    QWK::WindowBar* windowBar() const { return m_windowBar; }

signals:
    void themeChanged();

private slots:
    void onThemeChanged();

private:
    void setupTitleBar();
    void setupWindowAgent();
    void createMenuBar();
    void setupWindowButtons();
    void setupConnections();
    
    static void emulateLeaveEvent(QWidget *widget);

private:
    QMainWindow* m_window;
    QWK::WidgetWindowAgent* m_windowAgent;
    QWK::WindowBar* m_windowBar;
    QMenuBar* m_menuBar;
    QLabel* m_titleLabel;
    
    // 窗口控制按钮
#ifndef Q_OS_MAC
    QWK::WindowButton* m_iconButton;
    QWK::WindowButton* m_pinButton;
    QWK::WindowButton* m_minButton;
    QWK::WindowButton* m_maxButton;
    QWK::WindowButton* m_closeButton;
#endif

    ThemeManager::Type m_currentTheme;
}; 