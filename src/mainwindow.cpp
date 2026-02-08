#include "mainwindow.h"
#include <QMessageBox>
#include <fftw3.h>

#include "QtNativeUI/NPushButton.h"
#include "QtNativeUI/NTheme.h"

MainWindow::MainWindow(QWidget* parent) : NMainWindow(parent), m_updateTimer(new QTimer(this)) {
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    initializeUI();
    setupConnections();

    m_plot->replot();
    m_fftPlot->replot();

}

MainWindow::~MainWindow() = default;

void MainWindow::initializeUI() {

    setWindowTitle("QCustomPlot Demo");
    setWindowEffect(MicaAlt);
    setWindowIcon(style()->standardIcon(QStyle::SP_DesktopIcon));
    resize(800, 600);
    setStatusBar(nullptr);

    auto*        centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout    = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    setCentralWidget(centralWidget);


    m_button = new NPushButton("更新数据", this);
    mainLayout->addWidget(m_button);

    QHBoxLayout* plotLayout = new QHBoxLayout();
    mainLayout->addLayout(plotLayout);


    m_plot = new QCustomPlot(this);
    plotLayout->addWidget(m_plot);


    m_fftPlot = new QCustomPlot(this);
    plotLayout->addWidget(m_fftPlot);

    m_plot->addGraph();
    m_plot->graph(0)->setPen(QPen(Qt::blue));
    m_plot->xAxis->setLabel("Time (s)");
    m_plot->yAxis->setLabel("Amplitude");
    m_plot->xAxis->setRange(0, 20);
    m_plot->yAxis->setRange(-1.5, 1.5);


    m_fftPlot->addGraph();
    m_fftPlot->graph(0)->setPen(QPen(Qt::red, 2)); // 加粗线条
    m_fftPlot->xAxis->setLabel("Frequency (Hz)");
    m_fftPlot->yAxis->setLabel("Magnitude");
    m_fftPlot->xAxis->setRange(0, 50);
    m_fftPlot->yAxis->setRange(0, 0.6);

    m_fftPlot->xAxis->grid()->setSubGridVisible(true);
    m_fftPlot->yAxis->grid()->setSubGridVisible(true);
    m_fftPlot->xAxis->grid()->setSubGridPen(QColor(200, 200, 200));
    m_fftPlot->yAxis->grid()->setSubGridPen(QColor(200, 200, 200));

    m_fftPlot->legend->setVisible(true);
    m_fftPlot->graph(0)->setName("Frequency Spectrum");


    m_fftPlot->xAxis->setLabelFont(QFont("Arial", 10));
    m_fftPlot->yAxis->setLabelFont(QFont("Arial", 10));

    m_fftButton = new NPushButton("执行FFT", this);
    mainLayout->addWidget(m_fftButton);
}



void MainWindow::setupConnections() {
    connect(nTheme, &NTheme::darkModeChanged, this, &MainWindow::onThemeChanged);
    connect(m_button, &QPushButton::clicked, this, &MainWindow::updatePlot);
    connect(m_fftButton, &QPushButton::clicked, this, &MainWindow::performFFT);
}

void MainWindow::updatePlot() {

    m_timeData.clear();
    x.clear();


    QList<QCPItemText*> textLabels = m_fftPlot->findChildren<QCPItemText*>();
    for (auto label : textLabels) {
        m_fftPlot->removeItem(label);
    }


    m_plot->graph(0)->data()->clear();
    m_fftPlot->graph(0)->data()->clear();
    m_plot->replot();
    m_fftPlot->replot();

    double dt = 0.001;
    double T  = 20.0;

    for (double t = 0; t < T; t += dt) {
        x.append(t);
        double signal = 0.5 * sin(2 * M_PI * 2 * t) + 0.3 * sin(2 * M_PI * 5 * t) + 0.2 * sin(2 * M_PI * 10 * t) +
                        0.15 * sin(2 * M_PI * 15 * t) + 0.1 * sin(2 * M_PI * 20 * t);
        m_timeData.append(signal);
    }

    m_plot->graph(0)->setData(x, m_timeData);
    m_plot->replot();
}

void MainWindow::performFFT() {
    if (m_timeData.isEmpty()) {
        QMessageBox::warning(this, "错误", "FFT 功能需要数据才能正常工作，请先添加数据。");
        return;
    }

    QList<QCPItemText*> textLabels = m_fftPlot->findChildren<QCPItemText*>();
    for (auto label : textLabels) {
        m_fftPlot->removeItem(label);
    }

    int N = m_timeData.size();

    float*         in  = (float*) fftwf_malloc(sizeof(float) * N);
    fftwf_complex* out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (N / 2 + 1));

    for (int i = 0; i < N; i++) {
        // 汉宁窗函数: w(n) = 0.5 * (1 - cos(2π*n/(N-1)))
        float window = 0.5 * (1.0 - cos(2.0 * M_PI * i / (N - 1)));
        in[i]        = static_cast<float>(m_timeData[i]) * window;
    }

    fftwf_plan plan = fftwf_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
    fftwf_execute(plan);


    QVector<double> frequencies, magnitudes;
    double          fs = 1.0 / 0.001; // 采样频率 = 1/dt


    for (int i = 0; i < N / 2 + 1; i++) {
        double freq = i * fs / N;
        if (freq > 50)
            break;

        double mag = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / N;

        if (freq > 0 && freq <= 25) {
            mag *= 2.0;
        }

        if (mag > 0.005) {

            frequencies.append(freq);
            magnitudes.append(mag * 2);
        }
    }

    m_fftPlot->graph(0)->setData(frequencies, magnitudes);
    m_fftPlot->graph(0)->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::red, 7)); // 更大的圆形标记

    for (int i = 0; i < frequencies.size(); i++) {
        if (magnitudes[i] > 0.1) {
            QCPItemText* textLabel = new QCPItemText(m_fftPlot);
            textLabel->position->setType(QCPItemPosition::ptPlotCoords);
            textLabel->position->setCoords(frequencies[i], magnitudes[i] + 0.02);
            textLabel->setText(QString::number(frequencies[i], 'f', 1) + "Hz");
            textLabel->setFont(QFont("Arial", 8));
        }
    }

    double maxMag = *std::max_element(magnitudes.begin(), magnitudes.end());
    m_fftPlot->yAxis->setRange(0, maxMag * 1.2); // 留出 20% 的余量

    m_fftPlot->xAxis->grid()->setSubGridVisible(true);
    m_fftPlot->yAxis->grid()->setSubGridVisible(true);

    m_fftPlot->replot();


    fftwf_destroy_plan(plan);
    fftwf_free(in);
    fftwf_free(out);
}

void MainWindow::onThemeChanged(bool isDark) {
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
