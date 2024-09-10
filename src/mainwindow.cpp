//
// Created by admin on 2024/9/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include "ui_mainwindow.h"

mainwindow::mainwindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);
    ui->comboBoxChannel->setPlaceholderText("请选择通道");
    connect(ui->open_Board, &QPushButton::clicked, this, &mainwindow::openBoard);
    connect(ui->close_Board, &QPushButton::clicked, this, &mainwindow::closeBoard);
}

mainwindow::~mainwindow() { delete ui; }

void mainwindow::openBoard() {
    if (m_boardController.openBoard()) {
        statusBar()->showMessage("板卡成功打开");
        m_boardController.InterruptClear();
        m_boardController.ADCStop();
        m_boardController.Reset();
        m_boardController.SetTransmitMode(0, 0);
        for (int i = 0; i < 4; i++) {
            m_boardController.m_boardInterface->QT_BoardSetOffset12136DC(i);
        }
        showBoardInfo();
        setupChannel();


    } else {
        statusBar()->showMessage("板卡打开失败");
    }
}

void mainwindow::closeBoard() {
    m_boardController.closeBoard();
    statusBar()->showMessage("板卡已关闭");
}

void mainwindow::showBoardInfo() {

    const CardInfoData &cardInfo = m_boardController.getCardInfo();
    // 检查转换是否成功
    const QString cardType = QString::fromStdString(cardInfo.cardType);
    const QString cardLogicalVersion = QString::fromStdString(cardInfo.cardLogicalVersion);
    const QString sampleRate = QString::number(cardInfo.sampleRate);
    const QString channelNumber = QString::number(cardInfo.channelNumber);
    const QString resolution = QString::number(cardInfo.resolution);
    const QString cardStatus = QString::number(cardInfo.cardStatus);


    QMessageBox::information(this, "板卡信息",
                             QString("卡类型: %1\n卡逻辑版本: %2\n采样率: %3\n通道数: %4\n分辨率: %5\n卡状态: %6")
                                     .arg(cardType)
                                     .arg(cardLogicalVersion)
                                     .arg(sampleRate)
                                     .arg(channelNumber)
                                     .arg(resolution)
                                     .arg(cardStatus));

    ui->statusbar->showMessage(QString("卡类型: %1\n卡逻辑版本: %2\n采样率: %3\n通道数: %4\n分辨率: %5\n卡状态: %6")
                                       .arg(cardType)
                                       .arg(cardLogicalVersion)
                                       .arg(sampleRate)
                                       .arg(channelNumber)
                                       .arg(resolution)
                                       .arg(cardStatus));
}

void mainwindow::setupChannel() {
    const std::vector<ChannelInfo> &channelInfo = m_boardController.getCardInfo().channels;
    for (size_t i = 0; i < channelInfo.size(); ++i) {
        ui->comboBoxChannel->addItem(QString::number(i + 1));
    }
}
