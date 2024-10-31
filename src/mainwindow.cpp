#include "mainwindow.h"
#include <ElaTheme.h>
#include <QTimer>
#include <QVBoxLayout>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : ElaWindow(parent), ui(new Ui::MainWindow), m_updateTimer(new QTimer(this)) {
    ui->setupUi(this);
    initializeUI();
    setupConnections();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initializeUI() {
    // 设置窗口标题和大小
    setWindowTitle("ElaWidgetTools & QCustomPlot Demo");
    resize(800, 600);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 创建按钮
    m_button = new ElaPushButton("更新数据", this);
    mainLayout->addWidget(m_button);

    // 创建图表
    m_plot = new QCustomPlot(this);
    mainLayout->addWidget(m_plot);

    // 配置图表外观
    m_plot->addGraph();
    m_plot->graph(0)->setPen(QPen(Qt::blue));
    m_plot->xAxis->setLabel("Time (s)");
    m_plot->yAxis->setLabel("Value");
    m_plot->xAxis->setRange(0, 10);
    m_plot->yAxis->setRange(-1, 1);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // 设置窗口按钮
    setWindowButtonFlags(ElaAppBarType::MinimizeButtonHint | ElaAppBarType::MaximizeButtonHint |
                         ElaAppBarType::CloseButtonHint);
}

void MainWindow::setupConnections() {
    // 连接主题变更信号
    connect(eTheme, &ElaTheme::themeModeChanged, this, &MainWindow::onThemeChanged);

    // 连接按钮点击信号
    connect(m_button, &ElaPushButton::clicked, this, &MainWindow::updatePlot);
}

void MainWindow::updatePlot() {
    static QVector<double> x, y;
    if (x.size() > 100) {
        x.removeFirst();
        y.removeFirst();
    }

    // 添加新数据点
    x.append(x.isEmpty() ? 0 : x.last() + 0.1);
    y.append(qSin(x.last()));

    // 更新图表数据
    m_plot->graph(0)->setData(x, y);
    m_plot->replot();
}

void MainWindow::onThemeChanged(ElaThemeType::ThemeMode mode) {
    // 根据主题更新图表样式
    QPalette pal = m_plot->palette();
    bool isDark = (mode == ElaThemeType::Dark);

    // 设置背景色
    pal.setColor(QPalette::Window, isDark ? QColor(32, 32, 32) : Qt::white);
    m_plot->setPalette(pal);

    // 设置轴和网格颜色
    QColor textColor = isDark ? Qt::white : Qt::black;
    m_plot->xAxis->setLabelColor(textColor);
    m_plot->yAxis->setLabelColor(textColor);
    m_plot->xAxis->setTickLabelColor(textColor);
    m_plot->yAxis->setTickLabelColor(textColor);

    m_plot->replot();
}
