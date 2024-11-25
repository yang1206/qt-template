#include "mainwindow.h"
#include <QIcon>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <complex>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_plot(nullptr), m_fftPlot(nullptr) {
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    // 安装窗口代理
    installWindowAgent();

    setWindowIcon(QIcon(":/icon/icon/windows/app.ico"));
    initializeUI();
    setupConnections();
    setupThemeDetection();
    // 初始化空图表
    m_plot->replot();
    m_fftPlot->replot();
}

MainWindow::~MainWindow() = default;


void MainWindow::installWindowAgent() {
    // 1. 创建并设置窗口代理
    windowAgent = new QWK::WidgetWindowAgent(this);
    windowAgent->setup(this);

    // 2. 创建标题标签
    auto titleLabel = new QLabel(tr("QT Demo"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("win-title-label");

    // 3. 创建WindowBar
    auto windowBar = new QWK::WindowBar(this);
    windowBar->setTitleLabel(titleLabel);
    windowBar->setHostWidget(this);

#ifndef Q_OS_MAC
    // 非Mac平台：创建自定义窗口按钮
    auto iconButton = new QWK::WindowButton(this);
    iconButton->setObjectName("icon-button");
    iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto minButton = new QWK::WindowButton(this);
    minButton->setObjectName("min-button");
    minButton->setProperty("system-button", true);
    minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto maxButton = new QWK::WindowButton(this);
    maxButton->setObjectName("max-button");
    maxButton->setProperty("system-button", true);
    maxButton->setCheckable(true);
    maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto closeButton = new QWK::WindowButton(this);
    closeButton->setObjectName("close-button");
    closeButton->setProperty("system-button", true);
    closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 设置按钮到WindowBar
    windowBar->setIconButton(iconButton);
    windowBar->setMinButton(minButton);
    windowBar->setMaxButton(maxButton);
    windowBar->setCloseButton(closeButton);

    // 设置系统按钮
    windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, iconButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);

    // 连接按钮信号
    connect(windowBar, &QWK::WindowBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(windowBar, &QWK::WindowBar::maximizeRequested, this, [this, maxButton](bool max) {
        if (max) {
            showMaximized();
        } else {
            showNormal();
        }
        QTimer::singleShot(0, maxButton,
                           [maxButton]() { QCoreApplication::postEvent(maxButton, new QEvent(QEvent::Leave)); });
    });
    connect(windowBar, &QWK::WindowBar::closeRequested, this, &QWidget::close);
#else
    // Mac平台：设置系统按钮区域
    windowAgent->setSystemButtonAreaCallback([](const QSize &size) {
        static constexpr const int width = 75;
        return QRect(QPoint(size.width() - width, 0), QSize(width, size.height()));
    });
#endif

    // 4. 设置标题栏
    windowAgent->setTitleBar(windowBar);
    setMenuWidget(windowBar);
}

void MainWindow::initializeUI() {
    // 创建一个widget作为中央部件
    const auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建主布局
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建按钮
    m_button = new QPushButton(tr("更新数据"), this);
    mainLayout->addWidget(m_button);

    // 添加水平布局来放置两个图表
    auto *plotLayout = new QHBoxLayout();
    mainLayout->addLayout(plotLayout);

    // 左侧时域图表
    m_plot = new QCustomPlot(this);
    plotLayout->addWidget(m_plot, 1); // 添加拉伸因子

    // 右侧频域图表
    m_fftPlot = new QCustomPlot(this);
    plotLayout->addWidget(m_fftPlot, 1); // 添加拉伸因子

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
    m_fftButton = new QPushButton(tr("执行FFT"), this);
    mainLayout->addWidget(m_fftButton);

    // 设置窗口属性
    setWindowTitle(tr("QT Demo"));
    resize(800, 600);
}

void MainWindow::setupThemeDetection() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // 使用新的 colorScheme API
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme scheme) {
        const bool isDark = (scheme == Qt::ColorScheme::Dark);
        updateTheme(isDark);
    });

    // 初始化主题
    bool isDark = (qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    updateTheme(isDark);
#else
    // 回退方案:通过比较调色板颜色判断
    auto checkTheme = [this]() {
        const QPalette defaultPalette;
        bool isDark = defaultPalette.color(QPalette::WindowText).lightness() >
                      defaultPalette.color(QPalette::Window).lightness();
        updatePlotsTheme(isDark);
    };

    // 监听调色板变化
    connect(qApp, &QApplication::paletteChanged, this, checkTheme);

    // 初始化主题
    checkTheme();
#endif
}

void MainWindow::setupConnections() {
    // 连接按钮点击信号
    connect(m_button, &QPushButton::clicked, this, &MainWindow::updatePlot);
    connect(m_fftButton, &QPushButton::clicked, this, &MainWindow::performFFT);
}

void MainWindow::updatePlot() {
    // 清除之前的数据
    m_timeData.clear();
    x.clear();

    // 清除频域图表的标签
    QList<QCPItemText *> textLabels = m_fftPlot->findChildren<QCPItemText *>();
    for (auto label: textLabels) {
        m_fftPlot->removeItem(label);
    }

    // 清除图表数据
    m_plot->graph(0)->data()->clear();
    m_fftPlot->graph(0)->data()->clear();
    m_plot->replot();
    m_fftPlot->replot();

    // 生成新的数据
    constexpr double dt = 0.001;
    constexpr double T = 20.0;

    for (double t = 0; t < T; t += dt) {
        x.append(t);
        const double signal = 0.5 * sin(2 * M_PI * 2 * t) + 0.3 * sin(2 * M_PI * 5 * t) + 0.2 * sin(2 * M_PI * 10 * t) +
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
        QMessageBox::warning(this, "错误", "FFT 功能需要数据才能正常工作，请先添加数据。", QMessageBox::Ok);
        return;
    }

    // 清除之前的频域标签
    QList<QCPItemText *> textLabels = m_fftPlot->findChildren<QCPItemText *>();
    for (auto label: textLabels) {
        m_fftPlot->removeItem(label);
    }

    int N = m_timeData.size();

    // 分配 FFTW 内存
    auto *in = static_cast<float *>(fftwf_malloc(sizeof(float) * N));
    auto *out = static_cast<fftwf_complex *>(fftwf_malloc(sizeof(fftwf_complex) * (N / 2 + 1)));

    // 应用汉宁窗
    for (int i = 0; i < N; i++) {
        // 汉宁窗函数: w(n) = 0.5 * (1 - cos(2π*n/(N-1)))
        float window = 0.5 * (1.0 - cos(2.0 * M_PI * i / (N - 1)));
        in[i] = static_cast<float>(m_timeData[i]) * window;
    }

    // 创建 FFTW 计划
    fftwf_plan plan = fftwf_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
    fftwf_execute(plan);

    // 准备频域数据
    QVector<double> frequencies, magnitudes;
    double fs = 1.0 / 0.001; // 采样频率 = 1/dt

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
            auto *textLabel = new QCPItemText(m_fftPlot);
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

void MainWindow::updateTheme(bool isDark)const {
    // 更新两个图表的主题
    QColor textColor = isDark ? Qt::white : Qt::black;
    QColor bgColor = isDark ? QColor(32, 32, 32) : Qt::white;

    auto updatePlotTheme = [textColor, bgColor](QCustomPlot *plot) {
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