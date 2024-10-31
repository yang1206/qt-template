#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ElaPushButton.h>
#include <ElaTabWidget.h>
#include <ElaToolBar.h>
#include <ElaWindow.h>
#include <QMainWindow>
#include <qcustomplot.h>

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

private:
    void initializeUI();
    void setupConnections();

private:
    Ui::MainWindow *ui;
    QCustomPlot *m_plot;
    ElaPushButton *m_button;
    QTimer *m_updateTimer;
};

#endif // MAINWINDOW_H
