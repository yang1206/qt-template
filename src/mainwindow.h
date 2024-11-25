#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWKWidgets/widgetwindowagent.h>
#include <qcustomplot.h>
#include "fftw3.h"
#include "widgets/windowbar.h"
#include "widgets/windowbutton.h"

QT_BEGIN_NAMESPACE namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updatePlot();
    void performFFT();

private:
    void initializeUI();
    void setupConnections();
    void setupThemeDetection();
    void installWindowAgent();
    void updateTheme(bool isDark) const;

private:
    QWK::WidgetWindowAgent *windowAgent{};
    QCustomPlot *m_plot;
    QCustomPlot *m_fftPlot;
    QPushButton *m_button{};
    QPushButton *m_fftButton{};
    QVector<double> m_timeData;
    QVector<double> x;
};

#endif // MAINWINDOW_H
