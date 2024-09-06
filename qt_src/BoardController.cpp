//
// Created by yang1206 on 2024/9/5.
//
#include "BoardController.h"
#include <QDebug>

BoardController::BoardController(QObject *parent) : QObject(parent) {
    // 构造函数
}

BoardController::~BoardController() {
    closeBoard(); // 析构时确保关闭板卡
}

bool BoardController::openBoard() {
    int iRet = QTXdmaOpenBoard(&m_cardInfo, 0);
    if (iRet == 0) {
        emit boardOpened();
        qDebug() << "板卡成功打开";
        return true;
    } else {
        emit errorOccurred(QString("打开板卡失败，错误代码: %1").arg(iRet));
        return false;
    }
}

void BoardController::closeBoard() {
    QTXdmaCloseBoard(&m_cardInfo);
    emit boardClosed();
    qDebug() << "板卡已关闭";
}

QString BoardController::getBoardInfo() {
    uint64_t boardInfo;
    if (QTXdmaReadRegister(&m_cardInfo, BASE_BOARD_INFO, OFFSET_BDINFO_BDINFO, &boardInfo) == 0) {
        return QString("板卡信息: 0x%1 (%2)").arg(boardInfo, 0, 16).arg(QString::number(boardInfo, 10));
    }
    emit errorOccurred("读取板卡信息失败");
    return "读取板卡信息失败";
}

QString BoardController::getSoftwareVersion() {
    uint64_t softwareVer;
    if (QTXdmaReadRegister(&m_cardInfo, BASE_BOARD_INFO, OFFSET_BDINFO_SOFT_VER, &softwareVer) == 0) {
        return QString("软件版本: 0x%1 (%2)").arg(softwareVer, 0, 16).arg(QString::number(softwareVer, 10));
    }
    emit errorOccurred("读取软件版本失败");
    return "读取软件版本失败";
}

QString BoardController::getXdmaInfo() {
    uint64_t xdmaInfo;
    if (QTXdmaReadRegister(&m_cardInfo, BASE_BOARD_INFO, OFFSET_BDINFO_XDMA, &xdmaInfo) == 0) {
        return QString("XDMA信息: 0x%1 (%2)").arg(xdmaInfo, 0, 16).arg(QString::number(xdmaInfo, 10));
    }
    emit errorOccurred("读取XDMA信息失败");
    return "读取XDMA信息失败";
}

QString BoardController::getAdcInfo() {
    uint64_t adcInfo;
    if (QTXdmaReadRegister(&m_cardInfo, BASE_BOARD_INFO, OFFSET_BDINFO_ADC, &adcInfo) == 0) {
        uint32_t adcValue = static_cast<uint32_t>(adcInfo);
        uint32_t sampleRate = adcValue / 10000;  // 采样率
        uint32_t channelNumber = (adcValue / 100) % 100;  // 通道数
        uint32_t resolution = adcValue % 100;  // 分辨率
        return QString("ADC信息: 采样率: %1 Hz, 通道数: %2, 分辨率: %3 bit")
                .arg(sampleRate).arg(channelNumber).arg(resolution);
    }
    emit errorOccurred("读取ADC信息失败");
    return "读取ADC信息失败";
}


QString BoardController::getDacInfo() {
    uint64_t dacInfo;
    if (QTXdmaReadRegister(&m_cardInfo, BASE_BOARD_INFO, OFFSET_BDINFO_DAC, &dacInfo) == 0) {
        return QString("DAC信息: 0x%1 (%2)").arg(dacInfo, 0, 16).arg(QString::number(dacInfo, 10));
    }
    emit errorOccurred("读取DAC信息失败");
    return "读取DAC信息失败";
}


bool BoardController::fetchData(uint64_t offsetAddr, uint32_t length, QByteArray &outData) {
    uint8_t *buffer = new uint8_t[length];
    if (buffer == nullptr) {
        emit errorOccurred("内存分配失败");
        return false;
    }

    memset(buffer, 0, length);

    int result = QTXdmaGetDataBuffer(offsetAddr, &m_cardInfo, buffer, length, 1000);
    if (result == 0) {
        outData.clear();
        outData.append(reinterpret_cast<const char *>(buffer), length);
        delete[] buffer;
        emit dataReady(outData);  // 触发数据准备好信号
        return true;
    } else {
        emit errorOccurred(QString("获取数据失败，错误代码: %1").arg(result));
        delete[] buffer;
        return false;
    }
}


bool BoardController::setTriggerMode(uint32_t mode, uint32_t trigCount, uint32_t pulsePeriod, uint32_t pulseWidth) {
    int result = QTXdmaSetInternalPulseTrigger(&m_cardInfo, pulsePeriod, pulseWidth, trigCount);
    if (result == 0) {
        return true;
    }
    emit errorOccurred("设置触发模式失败");
    return false;
}

