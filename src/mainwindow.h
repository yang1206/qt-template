#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCustomPlot.h>

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

  private:
    QCustomPlot*      m_plot;
    QCustomPlot*      m_fftPlot;
    QPushButton*      m_button;
    QPushButton*      m_fftButton;
    QTimer*           m_updateTimer;
    QVector<double>   m_timeData;
    QVector<double>   x;
};

#endif // MAINWINDOW_H
