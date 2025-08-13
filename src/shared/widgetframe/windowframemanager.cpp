#include "windowframemanager.h"

#include <QApplication>
#include <QFile>
#include <QHoverEvent>
#include <QStyle>
#include <QTimer>
#include "windowbar.h"
#include "windowbutton.h"

WindowFrameManager::WindowFrameManager(QMainWindow* window)
    : QObject(window)
    , m_window(window)
    , m_windowAgent(nullptr)
    , m_windowBar(nullptr)
    , m_menuBar(nullptr)
    , m_titleLabel(nullptr)
#ifndef Q_OS_MAC
    , m_iconButton(nullptr)
    , m_pinButton(nullptr)
    , m_minButton(nullptr)
    , m_maxButton(nullptr)
    , m_closeButton(nullptr)
#endif
    , m_currentTheme(ThemeManager::Type::System)
{
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &WindowFrameManager::onThemeChanged);
}

WindowFrameManager::~WindowFrameManager() = default;

void WindowFrameManager::setupWindowFrame()
{
    setupWindowAgent();
    setupTitleBar();
    setupConnections();
    
    // 初始加载当前主题
    loadTheme(ThemeManager::instance().currentTheme());
}

void WindowFrameManager::loadTheme(ThemeManager::Type theme)
{
    if (theme == m_currentTheme && !m_window->styleSheet().isEmpty())
        return;
    ThemeManager::instance().setTheme(theme);
    m_currentTheme = theme;
    bool isDark = (theme == ThemeManager::Type::Dark) || 
                 (theme == ThemeManager::Type::System && ThemeManager::instance().isDarkMode());
    
    QString qssPath = isDark ? QStringLiteral(":/qwk/dark-style.qss") 
                             : QStringLiteral(":/qwk/light-style.qss");
                             
    QFile qss(qssPath);
    if (qss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_window->setStyleSheet(QString::fromUtf8(qss.readAll()));
        emit themeChanged();
    }
}

void WindowFrameManager::setupWindowAgent()
{
    m_windowAgent = new QWK::WidgetWindowAgent(m_window);
    m_windowAgent->setup(m_window);
}

void WindowFrameManager::setupTitleBar()
{
    createMenuBar();
    
    m_titleLabel = new QLabel();
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setObjectName(QStringLiteral("win-title-label"));
    
    m_windowBar = new QWK::WindowBar();

#ifndef Q_OS_MAC
    setupWindowButtons();
    
    m_windowBar->setIconButton(m_iconButton);
    m_windowBar->setPinButton(m_pinButton);
    m_windowBar->setMinButton(m_minButton);
    m_windowBar->setMaxButton(m_maxButton);
    m_windowBar->setCloseButton(m_closeButton);
#endif

    m_windowBar->setMenuBar(m_menuBar);
    m_windowBar->setTitleLabel(m_titleLabel);
    m_windowBar->setHostWidget(m_window);

    m_windowAgent->setTitleBar(m_windowBar);
    
#ifndef Q_OS_MAC
    m_windowAgent->setHitTestVisible(m_pinButton, true);
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, m_iconButton);
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, m_minButton);
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, m_maxButton);
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::Close, m_closeButton);
#endif

    m_windowAgent->setHitTestVisible(m_menuBar, true);

#ifdef Q_OS_MAC
    m_windowAgent->setSystemButtonAreaCallback([](const QSize &size) {
        static constexpr const int width = 75;
        return QRect(QPoint(size.width() - width, 0), QSize(width, size.height()));
    });
#endif

    m_window->setMenuWidget(m_windowBar);
}

void WindowFrameManager::createMenuBar()
{
    m_menuBar = new QMenuBar(m_window);
    m_menuBar->setObjectName(QStringLiteral("win-menu-bar"));
}

#ifndef Q_OS_MAC
void WindowFrameManager::setupWindowButtons()
{
    m_iconButton = new QWK::WindowButton();
    m_iconButton->setObjectName(QStringLiteral("icon-button"));
    m_iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_pinButton = new QWK::WindowButton();
    m_pinButton->setCheckable(true);
    m_pinButton->setObjectName(QStringLiteral("pin-button"));
    m_pinButton->setProperty("system-button", true);
    m_pinButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_minButton = new QWK::WindowButton();
    m_minButton->setObjectName(QStringLiteral("min-button"));
    m_minButton->setProperty("system-button", true);
    m_minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_maxButton = new QWK::WindowButton();
    m_maxButton->setCheckable(true);
    m_maxButton->setObjectName(QStringLiteral("max-button"));
    m_maxButton->setProperty("system-button", true);
    m_maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_closeButton = new QWK::WindowButton();
    m_closeButton->setObjectName(QStringLiteral("close-button"));
    m_closeButton->setProperty("system-button", true);
    m_closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}
#endif

void WindowFrameManager::setupConnections()
{
#ifndef Q_OS_MAC
    connect(m_windowBar, &QWK::WindowBar::pinRequested, this, [this](bool pin){
        if (m_window->isHidden() || m_window->isMinimized() || 
            m_window->isMaximized() || m_window->isFullScreen()) {
            return;
        }
        m_window->setWindowFlag(Qt::WindowStaysOnTopHint, pin);
        m_window->show();
        m_pinButton->setChecked(pin);
    });
    
    connect(m_windowBar, &QWK::WindowBar::minimizeRequested, m_window, &QWidget::showMinimized);
    
    connect(m_windowBar, &QWK::WindowBar::maximizeRequested, this, [this](bool max) {
        if (max) {
            m_window->showMaximized();
        } else {
            m_window->showNormal();
        }

        // 修复鼠标悬停问题
        emulateLeaveEvent(m_maxButton);
    });
    
    connect(m_windowBar, &QWK::WindowBar::closeRequested, m_window, &QWidget::close);
#endif
}

void WindowFrameManager::onThemeChanged()
{
    loadTheme(ThemeManager::instance().currentTheme());
}

void WindowFrameManager::emulateLeaveEvent(QWidget *widget) 
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    
    QTimer::singleShot(0, widget, [widget]() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QScreen *screen = widget->screen();
#else
        const QScreen *screen = widget->windowHandle()->screen();
#endif
        const QPoint globalPos = QCursor::pos(screen);
        if (!QRect(widget->mapToGlobal(QPoint{0, 0}), widget->size()).contains(globalPos)) {
            QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
            if (widget->testAttribute(Qt::WA_Hover)) {
                const QPoint localPos = widget->mapFromGlobal(globalPos);
                const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
                static constexpr const auto oldPos = QPoint{};
                const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
                const auto event =
                    new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
                Q_UNUSED(localPos);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#else
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#endif
                QCoreApplication::postEvent(widget, event);
            }
        }
    });
} 