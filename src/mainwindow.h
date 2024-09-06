//
// Created by admin on 2024/9/2.
//

#ifndef QT_TEMPLATE_MAINWINDOW_H
#define QT_TEMPLATE_MAINWINDOW_H

#include <QMainWindow>
#include "../qt_src/BoardController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class mainwindow;
}
QT_END_NAMESPACE

class mainwindow : public QMainWindow {
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr);
    ~mainwindow() override;
private slots:
    void openBoard();    // 打开板卡的槽函数
    void closeBoard();   // 关闭板卡的槽函数
    void showBoardInfo();   // 显示板卡信息

private:
    Ui::mainwindow *ui;
    BoardController m_boardController; // 添加板卡控制器
};


#endif // QT_TEMPLATE_MAINWINDOW_H
