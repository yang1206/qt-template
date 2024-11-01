#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ElaPushButton.h>
#include <ElaTabWidget.h>
#include <ElaToolBar.h>
#include <ElaWindow.h>
#include <QMainWindow>
#include <qcustomplot.h>
#include "fftw3.h"

QT_BEGIN_NAMESPACE namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public ElaWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updatePlot();
    void onThemeChanged(ElaThemeType::ThemeMode mode);
    void performFFT();

private:
    void initializeUI();
    void setupConnections();
    void setupFFTPlot();

private:
    Ui::MainWindow *ui;
    QCustomPlot *m_plot;
    QCustomPlot *m_fftPlot;
    ElaPushButton *m_button;
    ElaPushButton *m_fftButton;
    QTimer *m_updateTimer;
    QVector<double> m_timeData;
    QVector<double> x;
};

#endif // MAINWINDOW_H
