//
// Created by admin on 2024/9/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

mainwindow::mainwindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);
    connect(ui->open_Board, &QPushButton::clicked, this, &mainwindow::openBoard);
    connect(ui->close_Board, &QPushButton::clicked, this, &mainwindow::closeBoard);
}

mainwindow::~mainwindow() { delete ui; }

void mainwindow::openBoard() {
    if (m_boardController.openBoard()) {
        statusBar()->showMessage("板卡成功打开");
        showBoardInfo();

    } else {
        statusBar()->showMessage("板卡打开失败");
    }
}

void mainwindow::closeBoard() {
    m_boardController.closeBoard();
    statusBar()->showMessage("板卡已关闭");
}

void mainwindow::showBoardInfo() {
    // 将板卡信息显示到界面或statusBar
    QString info = m_boardController.getBoardInfo() + "\n" +
                   m_boardController.getSoftwareVersion() + "\n" +
                   m_boardController.getXdmaInfo() + "\n" +
                   m_boardController.getAdcInfo() + "\n" +
                   m_boardController.getDacInfo();
    QMessageBox::information(this, "板卡信息", info); // 或者用 MessageBox 显示详细信息
    ui->statusbar->showMessage(info);
}