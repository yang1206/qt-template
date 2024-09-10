//
// Created by yang1206 on 2024/9/5.
//

#ifndef QT_TEMPLATE_BOARDCONTROLLER_H
#define QT_TEMPLATE_BOARDCONTROLLER_H
#include <QObject>
#include <QString>
#include "BoardHardwareInterface.h"

class BoardController : public QObject {
    Q_OBJECT
public:
    explicit BoardController(QObject *parent = nullptr);
    ~BoardController() override;

    bool openBoard(); // 打开板卡
    bool closeBoard(); // 关闭板卡


    CardInfoData getCardInfo(); // 获取卡信息

    void InterruptClear(); // 获取中断清除

    void ADCStop(); // 停止ADC
    void Reset(); // 复位
    void SetTransmitMode(int transmitpoints, int transmittimes);
    BoardHardwareInterface *m_boardInterface; // BoardHardwareInterface 的实例
    

private:
protected:
};


#endif // QT_TEMPLATE_BOARDCONTROLLER_H
