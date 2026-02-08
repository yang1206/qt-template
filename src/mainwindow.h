#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCustomPlot.h>
#include "QtNativeUI/NMainWindow.h"

class NPushButton;
class MainWindow : public NMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    void updatePlot();
    void onThemeChanged(bool dark);
    void performFFT();

  private:
    void initializeUI();
    void setupConnections();

  private:
    QCustomPlot*      m_plot;
    QCustomPlot*      m_fftPlot;
    NPushButton*      m_button;
    NPushButton*      m_fftButton;
    QTimer*           m_updateTimer;
    QList<double>   m_timeData;
    QList<double>   x;
};

#endif // MAINWINDOW_H
