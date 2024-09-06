//
// Created by yang1206 on 2024/9/5.
//

#ifndef QT_TEMPLATE_BOARDCONTROLLER_H
#define QT_TEMPLATE_BOARDCONTROLLER_H


#include "QTXdmaApi.h"
#include <QObject>
#include <QString>

class BoardController : public QObject {
    Q_OBJECT
public:
    explicit BoardController(QObject *parent = nullptr);
    ~BoardController() override;

    bool openBoard();            // 打开板卡
    void closeBoard();           // 关闭板卡


    QString getBoardInfo();      // 获取板卡信息
    QString getSoftwareVersion();// 获取软件版本
    QString getXdmaInfo();       // 获取XDMA信息
    QString getAdcInfo();        // 获取ADC信息
    QString getDacInfo();        // 获取DAC信息

    // 数据获取
    bool fetchData(uint64_t offsetAddr, uint32_t length, QByteArray &outData);

    // 设置触发模式
    bool setTriggerMode(uint32_t mode, uint32_t trigCount, uint32_t pulsePeriod, uint32_t pulseWidth);

    // 使能中断
    bool enableInterrupt();

    // 软件触发
    bool softwareTrigger();

    // 停止ADC采集
    bool stopAdc();

signals:
    void boardOpened();
    void boardClosed();
    void dataReady(const QByteArray &data); // 数据准备好
    void errorOccurred(const QString &errorMsg);


private:
    STXDMA_CARDINFO m_cardInfo{};  // 存放PCIE板卡信息结构体
};


#endif // QT_TEMPLATE_BOARDCONTROLLER_H
