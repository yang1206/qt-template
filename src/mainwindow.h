#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>
#include <ElaPushButton.h>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    void updatePlot();
    void onThemeChanged();
    void performFFT();

  private:
    void initializeUI();
    void setupConnections();
    void setupFFTPlot();

  private:
    QCustomPlot*    m_plot;
    QCustomPlot*    m_fftPlot;
    ElaPushButton*  m_button;
    ElaPushButton*  m_fftButton;
    QTimer*         m_updateTimer;
    QVector<double> m_timeData;
    QVector<double> x;
};

#endif // MAINWINDOW_H
