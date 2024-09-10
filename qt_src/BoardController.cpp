//
// Created by yang1206 on 2024/9/5.
//
#include "BoardController.h"
#include <QDebug>

BoardController::BoardController(QObject *parent) : QObject(parent) {
    // 构造函数，创建 BoardHardwareInterface 实例
    m_boardInterface = new BoardHardwareInterface();
}

BoardController::~BoardController() {
    // 析构函数，释放 BoardHardwareInterface 实例
    delete m_boardInterface;
}

bool BoardController::openBoard() { return m_boardInterface->QT_BoardOpen(); }

bool BoardController::closeBoard() { return m_boardInterface->QT_BoardClose(); }


CardInfoData BoardController::getCardInfo() { return m_boardInterface->QT_BoardGetCardInfo(); }
void BoardController::InterruptClear() {
    int ret = m_boardInterface->QT_BoardInterruptClear();
    if (ret != 0) {
        qWarning() << "PCIe中断清除清除失败，错误代码:" << ret;
    } else {
        qDebug() << "PCIe中断清除清除成功";
    }
}

void BoardController::ADCStop() {
    int ret = m_boardInterface->QT_BoardADCStop();
    if (ret != 0) {
        qWarning() << "ADC停止失败，错误代码:" << ret;
    } else {
        qDebug() << "ADC停止成功";
    }
}
void BoardController::Reset() {
    int ret = m_boardInterface->QT_BoardReset();
    if (ret != 0) {
        qWarning() << "重置失败，错误代码:" << ret;
    } else {
        qDebug() << "软件复位成功";
    }
}
void BoardController::SetTransmitMode(int transmitpoints, int transmittimes) {
    int ret = m_boardInterface->QT_BoardSetTransmitMode(transmitpoints, transmittimes);
    if (ret != 0) {
        qWarning() << "设置传输模式失败，错误代码:" << ret;
    } else {
        qDebug() << "设置传输模式成功";
    }
}
