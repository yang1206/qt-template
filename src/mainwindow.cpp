#include "mainwindow.h"
#include <QActionGroup>
#include <QMessageBox>
#include <QQmlContext>

#include <QQuickWidget>
#include <fftw3.h>
#include "shared/widgetframe/windowframemanager.h"
#include "utils/theme/theme_manager.h"
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), m_updateTimer(new QTimer(this)) {
    setAttribute(Qt::WA_DontCreateNativeAncestors);
#ifdef Q_OS_MAC
    setAttribute(Qt::WA_ContentsMarginsRespectsSafeArea, false);
#endif
    // 创建窗口框架管理器
    m_frameManager = new WindowFrameManager(this);
    m_frameManager->setupWindowFrame();

    initializeUI();
    setupMenus();
    setupConnections();

    // 初始化空图表
    m_plot->replot();
    m_fftPlot->replot();

    // 通知主题更新
    ThemeManager::instance().notifyThemeChange();
}

MainWindow::~MainWindow() = default;

void MainWindow::initializeUI() {
    // 设置窗口标题和大小
    setWindowTitle("QCustomPlot Demo");
    resize(800, 600);
    setStatusBar(nullptr);

    // 创建主布局
    auto*        centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout    = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    setCentralWidget(centralWidget);

    // 创建按钮
    m_button = new QPushButton("更新数据", this);
    mainLayout->addWidget(m_button);

    // 添加水平布局来放置两个图表
    QHBoxLayout* plotLayout = new QHBoxLayout();
    mainLayout->addLayout(plotLayout);

    // 左侧时域图表
    m_plot = new QCustomPlot(this);
    plotLayout->addWidget(m_plot);

    // 右侧频域图表
    m_fftPlot = new QCustomPlot(this);
    plotLayout->addWidget(m_fftPlot);

    // 配置时域图表
    m_plot->addGraph();
    m_plot->graph(0)->setPen(QPen(Qt::blue));
    m_plot->xAxis->setLabel("Time (s)");
    m_plot->yAxis->setLabel("Amplitude");
    m_plot->xAxis->setRange(0, 20);
    m_plot->yAxis->setRange(-1.5, 1.5);

    // 配置频域图表
    m_fftPlot->addGraph();
    m_fftPlot->graph(0)->setPen(QPen(Qt::red, 2)); // 加粗线条
    m_fftPlot->xAxis->setLabel("Frequency (Hz)");
    m_fftPlot->yAxis->setLabel("Magnitude");
    m_fftPlot->xAxis->setRange(0, 50);
    m_fftPlot->yAxis->setRange(0, 0.6);

    // 设置更精细的网格
    m_fftPlot->xAxis->grid()->setSubGridVisible(true);
    m_fftPlot->yAxis->grid()->setSubGridVisible(true);
    m_fftPlot->xAxis->grid()->setSubGridPen(QColor(200, 200, 200));
    m_fftPlot->yAxis->grid()->setSubGridPen(QColor(200, 200, 200));

    // 添加图例
    m_fftPlot->legend->setVisible(true);
    m_fftPlot->graph(0)->setName("Frequency Spectrum");

    // 优化轴标签
    m_fftPlot->xAxis->setLabelFont(QFont("Arial", 10));
    m_fftPlot->yAxis->setLabelFont(QFont("Arial", 10));

    // 添加 FFT 按钮
    m_fftButton = new QPushButton("执行FFT", this);
    mainLayout->addWidget(m_fftButton);
     auto  m_qmlPanel = new QQuickWidget(this);
    m_qmlPanel->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // 将MainWindow暴露给QML
    m_qmlPanel->rootContext()->setContextProperty("mainWindow", this);

    // 加载QML文件
    m_qmlPanel->setSource(QUrl("qrc:/qml/MyCustomWidget.qml"));
    mainLayout->addWidget(m_qmlPanel);
}

void MainWindow::setupMenus() {
    // 获取菜单栏
    QMenuBar* menuBar = m_frameManager->menuBar();
    if (!menuBar)
        return;

    // 创建设置菜单
    auto settings = new QMenu(tr("设置(&S)"), menuBar);

    // 添加主题切换动作
    auto darkAction = new QAction(tr("启用深色主题"), settings);
    darkAction->setCheckable(true);
    darkAction->setChecked(ThemeManager::instance().isDarkMode());
    connect(darkAction, &QAction::triggered, this, [this](bool checked) {
        m_frameManager->loadTheme(checked ? ThemeManager::Type::Dark : ThemeManager::Type::Light);
    });
    settings->addAction(darkAction);

    // 添加平台特定的窗口效果选项
    setupWindowEffects();

    // 将设置菜单添加到菜单栏
    menuBar->addMenu(settings);
}

void MainWindow::setupWindowEffects() {
    auto settings = m_frameManager->menuBar()->findChild<QMenu*>();
    if (!settings)
        return;

#ifdef Q_OS_WIN
    settings->addSeparator();

    auto noneAction = new QAction(tr("无特效"), settings);
    noneAction->setData(QStringLiteral("none"));
    noneAction->setCheckable(true);
    noneAction->setChecked(true);

    auto dwmBlurAction = new QAction(tr("DWM模糊"), settings);
    dwmBlurAction->setData(QStringLiteral("dwm-blur"));
    dwmBlurAction->setCheckable(true);

    auto acrylicAction = new QAction(tr("亚克力效果"), settings);
    acrylicAction->setData(QStringLiteral("acrylic-material"));
    acrylicAction->setCheckable(true);

    auto micaAction = new QAction(tr("云母效果"), settings);
    micaAction->setData(QStringLiteral("mica"));
    micaAction->setCheckable(true);

    auto micaAltAction = new QAction(tr("替代云母效果"), settings);
    micaAltAction->setData(QStringLiteral("mica-alt"));
    micaAltAction->setCheckable(true);

    auto winStyleGroup = new QActionGroup(settings);
    winStyleGroup->addAction(noneAction);
    winStyleGroup->addAction(dwmBlurAction);
    winStyleGroup->addAction(acrylicAction);
    winStyleGroup->addAction(micaAction);
    winStyleGroup->addAction(micaAltAction);

    connect(winStyleGroup, &QActionGroup::triggered, this, [this, winStyleGroup](QAction* action) {
        // 先重置所有样式
        for (const QAction* _act : winStyleGroup->actions()) {
            const QString data = _act->data().toString();
            if (data.isEmpty() || data == QStringLiteral("none")) {
                continue;
            }
            m_frameManager->windowAgent()->setWindowAttribute(data, false);
        }

        const QString data = action->data().toString();
        if (data == QStringLiteral("none")) {
            setProperty("custom-style", false);
        } else if (!data.isEmpty()) {
            m_frameManager->windowAgent()->setWindowAttribute(data, true);
            setProperty("custom-style", true);
        }
        style()->polish(this);
    });

    settings->addAction(noneAction);
    settings->addAction(dwmBlurAction);
    settings->addAction(acrylicAction);
    settings->addAction(micaAction);
    settings->addAction(micaAltAction);
#endif

#ifdef Q_OS_MAC
    // macOS特有的模糊效果选项
    auto darkBlurAction = new QAction(tr("深色模糊"), settings);
    darkBlurAction->setCheckable(true);
    connect(darkBlurAction, &QAction::toggled, this, [this](bool checked) {
        if (!m_frameManager->windowAgent()->setWindowAttribute(QStringLiteral("blur-effect"), "dark")) {
            return;
        }
        if (checked) {
            setProperty("custom-style", true);
            style()->polish(this);
        }
    });

    auto lightBlurAction = new QAction(tr("浅色模糊"), settings);
    lightBlurAction->setCheckable(true);
    connect(lightBlurAction, &QAction::toggled, this, [this](bool checked) {
        if (!m_frameManager->windowAgent()->setWindowAttribute(QStringLiteral("blur-effect"), "light")) {
            return;
        }
        if (checked) {
            setProperty("custom-style", true);
            style()->polish(this);
        }
    });

    auto noBlurAction = new QAction(tr("无模糊"), settings);
    noBlurAction->setCheckable(true);
    connect(noBlurAction, &QAction::toggled, this, [this](bool checked) {
        if (!m_frameManager->windowAgent()->setWindowAttribute(QStringLiteral("blur-effect"), "none")) {
            return;
        }
        if (checked) {
            setProperty("custom-style", false);
            style()->polish(this);
        }
    });

    auto macStyleGroup = new QActionGroup(settings);
    macStyleGroup->addAction(darkBlurAction);
    macStyleGroup->addAction(lightBlurAction);
    macStyleGroup->addAction(noBlurAction);

    settings->addAction(darkBlurAction);
    settings->addAction(lightBlurAction);
    settings->addAction(noBlurAction);
#endif
}

void MainWindow::setupConnections() {
    // 连接主题变更信号
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MainWindow::onThemeChanged);
    // 连接按钮点击信号
    connect(m_button, &QPushButton::clicked, this, &MainWindow::updatePlot);
    connect(m_fftButton, &QPushButton::clicked, this, &MainWindow::performFFT);
}

void MainWindow::updatePlot() {
    // 清除之前的数据
    m_timeData.clear();
    x.clear();

    // 清除频域图表的标签
    QList<QCPItemText*> textLabels = m_fftPlot->findChildren<QCPItemText*>();
    for (auto label : textLabels) {
        m_fftPlot->removeItem(label);
    }

    // 清除图表数据
    m_plot->graph(0)->data()->clear();
    m_fftPlot->graph(0)->data()->clear();
    m_plot->replot();
    m_fftPlot->replot();

    // 生成新的数据
    double dt = 0.001;
    double T  = 20.0;

    for (double t = 0; t < T; t += dt) {
        x.append(t);
        double signal = 0.5 * sin(2 * M_PI * 2 * t) + 0.3 * sin(2 * M_PI * 5 * t) + 0.2 * sin(2 * M_PI * 10 * t) +
                        0.15 * sin(2 * M_PI * 15 * t) + 0.1 * sin(2 * M_PI * 20 * t);
        m_timeData.append(signal);
    }

    // 更新时域图表
    m_plot->graph(0)->setData(x, m_timeData);
    m_plot->replot();
}

void MainWindow::performFFT() {
    // 检查是否有数据
    if (m_timeData.isEmpty()) {
        QMessageBox::warning(this, "错误", "FFT 功能需要数据才能正常工作，请先添加数据。");
        return;
    }

    // 清除之前的频域标签
    QList<QCPItemText*> textLabels = m_fftPlot->findChildren<QCPItemText*>();
    for (auto label : textLabels) {
        m_fftPlot->removeItem(label);
    }

    int N = m_timeData.size();

    // 分配 FFTW 内存
    float*         in  = (float*) fftwf_malloc(sizeof(float) * N);
    fftwf_complex* out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (N / 2 + 1));

    // 应用汉宁窗
    for (int i = 0; i < N; i++) {
        // 汉宁窗函数: w(n) = 0.5 * (1 - cos(2π*n/(N-1)))
        float window = 0.5 * (1.0 - cos(2.0 * M_PI * i / (N - 1)));
        in[i]        = static_cast<float>(m_timeData[i]) * window;
    }

    // 创建 FFTW 计划
    fftwf_plan plan = fftwf_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
    fftwf_execute(plan);

    // 准备频域数据
    QVector<double> frequencies, magnitudes;
    double          fs = 1.0 / 0.001; // 采样频率 = 1/dt

    // 计算频谱
    for (int i = 0; i < N / 2 + 1; i++) {
        double freq = i * fs / N;
        if (freq > 50)
            break; // 只显示 50Hz 以下的频率

        // 计算幅度谱并归一化
        double mag = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / N;

        // 对低频分量进行加强显示
        if (freq > 0 && freq <= 25) {
            // 0-25Hz 范围内
            mag *= 2.0; // 加强低频显示
        }

        // 添加频率点（使用对数平滑）
        if (mag > 0.005) {
            // 提高阈值以减少噪声
            frequencies.append(freq);
            magnitudes.append(mag * 2); // 补偿单边谱
        }
    }

    // 更新频域图表
    m_fftPlot->graph(0)->setData(frequencies, magnitudes);
    m_fftPlot->graph(0)->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::red, 7)); // 更大的圆形标记

    // 添加峰值标签
    for (int i = 0; i < frequencies.size(); i++) {
        if (magnitudes[i] > 0.1) {
            // 只标记主要峰值
            QCPItemText* textLabel = new QCPItemText(m_fftPlot);
            textLabel->position->setType(QCPItemPosition::ptPlotCoords);
            textLabel->position->setCoords(frequencies[i], magnitudes[i] + 0.02);
            textLabel->setText(QString::number(frequencies[i], 'f', 1) + "Hz");
            textLabel->setFont(QFont("Arial", 8));
        }
    }

    // 自动调整 Y 轴范围
    double maxMag = *std::max_element(magnitudes.begin(), magnitudes.end());
    m_fftPlot->yAxis->setRange(0, maxMag * 1.2); // 留出 20% 的余量

    // 设置网格
    m_fftPlot->xAxis->grid()->setSubGridVisible(true);
    m_fftPlot->yAxis->grid()->setSubGridVisible(true);

    m_fftPlot->replot();

    // 清理 FFTW 资源
    fftwf_destroy_plan(plan);
    fftwf_free(in);
    fftwf_free(out);
}

void MainWindow::onThemeChanged() {
    auto& theme  = ThemeManager::instance();
    bool  isDark = theme.isDarkMode();
    // 更新两个图表的主题
    QColor textColor = isDark ? Qt::white : Qt::black;
    QColor bgColor   = isDark ? QColor(32, 32, 32) : Qt::white;

    auto updatePlotTheme = [textColor, bgColor](QCustomPlot* plot) {
        plot->setBackground(bgColor);
        plot->xAxis->setLabelColor(textColor);
        plot->yAxis->setLabelColor(textColor);
        plot->xAxis->setTickLabelColor(textColor);
        plot->yAxis->setTickLabelColor(textColor);
        plot->replot();
    };

    updatePlotTheme(m_plot);
    updatePlotTheme(m_fftPlot);
}
