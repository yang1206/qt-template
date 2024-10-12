//
// Created by admin on 2024/9/6.
//

#ifndef QT_TEMPLATE_BOARDHARDWAREINTERFACE_H
#define QT_TEMPLATE_BOARDHARDWAREINTERFACE_H


#include <QString>
#include <cstdint>
#include "QTXdmaApi.h"

struct ChannelInfo {
    int channelId; // 通道ID, 比如0,1,2,3
    uint32_t offsetValue; // 通道偏置下发数值
};


struct CardInfoData {
    std::string cardType; // 卡类型
    std::string cardLogicalVersion; // 卡逻辑版本
    uint32_t sampleRate; // 采样率
    uint32_t channelNumber; // 通道数
    uint32_t resolution; // 分辨率
    uint32_t cardStatus; // 卡状态
    std::vector<ChannelInfo> channels; // 存储每个通道的信息
};


class BoardHardwareInterface {

public:
    BoardHardwareInterface();
    ~BoardHardwareInterface();

    CardInfoData QT_BoardGetCardInfo();
    bool QT_BoardOpen();
    bool QT_BoardClose();
    // 清除中断
    int QT_BoardInterruptClear();
    int QT_BoardADCStop();
    int QT_BoardReset();
    int QT_BoardSetChannelOffset(uint32_t channelId, uint32_t offsetValue);
    /**
     * 预触发设置
     * @param pre_trig_length 预触发点数
     * @return
     */
    int QT_BoardSetPerTrigger(uint32_t pre_trig_length);
    /**
     * 帧头设置
     * @param frameheaderenable 帧头开关 0:关闭 1:开启
     * @return
     */
    int QT_BoardSetFrameheader(uint32_t frameheaderenable);
    /**
     * 时钟模式设置
     * @param clockmode 时钟模式 0:内参考时钟 1:外采样时钟 2:外参考时钟
     * @return
     */
    int QT_BoardSetClockMode(uint32_t clockmode);
    /**
     *设置DMA搬运数据量
     */
    int QT_BoardMultiTrigerSingleDma(uint32_t DMAoncebytes, uint32_t once_trig_bytes, uint32_t DAMTotolbytes);
    // 函数功能: 设置DMA传输参数
    // 函数参数：once_trig_bytes：单次触发数据量   DMATotolbytes：中断数据量
    // 函数返回: 成功返回0,失败返回-1并把详细错误信息写入日志文件
    int QT_BoardSetStdSingleDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes);

    // 函数功能: 设置StdMulti DMA传输参数
    // 函数参数：once_trig_bytes：单次触发数据量   DMATotolbytes：中断数据量
    // 函数返回: 成功返回0,失败返回-1并把详细错误信息写入日志文件
    int QT_BoardSetStdMultiDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes);

    // 函数功能: 设置FifoSingle DMA传输参数
    // 函数参数：once_trig_bytes：单次触发数据量   DMATotolbytes：中断数据量
    // 函数返回: 成功返回0,失败返回-1并把详细错误信息写入日志文件
    int QT_BoardSetFifoSingleDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes);

    // 函数功能: 设置FifoMulti DMA传输参数
    // 函数参数：once_trig_bytes：单次触发数据量   DMATotolbytes：中断数据量
    // 函数返回: 成功返回0,失败返回-1并把详细错误信息写入日志文件
    int QT_BoardSetFifoMultiDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes);
    // 设置传输模式为有限次传输模式
    // transmitpoints：0为有限点 1为无限点
    // transmittimes：DMA0传输次数 0有限次 1无限次
    int QT_BoardSetTransmitMode(int transmitpoints, int transmittimes);
    /**
     * 软件触发
     * @return
     */
    int QT_BoardSoftTrigger();
    /**
     * @brief 内部脉冲触发
     *
     * @param[in] counts 	设置触发次数。
     * @param[in] pulse_period 设置秒脉冲触发周期。
     * @param[in] pulse_width 设置秒脉冲触发宽度。
     */
    int QT_BoardPulseTrigger(int counts, int pulse_period, int pulse_width);
    /**
     * @brief 外部上升沿触发
     */
    int QT_BoardExtrigger_rising(int counts);
    /**
     * @brief 外部脉冲下降沿触发
     */
    int QT_BoardExtrigger_falling(int counts);
    /**
     * @brief 通道触发
     *
     * @param[in] mode 设置通道触发触发模式。
     * @param[in] counts 	设置触发次数。
     * @param[in] channelID 设置通道触发触发通道ID号。
     * @param[in] rasing_codevalue 设置通道上升沿触发电平。
     * @param[in] falling_codevalue 设置通道下降沿触发电平。
     */
    int QT_BoardChannelTrigger(int mode, int counts, int channelID, int rasing_codevalue, int falling_codevalue);
    // 函数功能: 外部脉冲触发
    // 函数参数：mode：触发模式  counts：触发次数
    // 函数返回: 成功返回0,失败返回-1
    int QT_BoardExternalTrigger(int mode, int counts);
    // 使能PCIE中断
    int QT_BoardSetInterruptSwitch();
    // 中断模式
    void QT_BoardInterruptGatherType();
    // 轮询模式
    void QT_BoardPollingModeGatherType();
    // 开始采集
    int QT_BoardSetADCStart();
    // 停止采集
    int QT_BoardSetADCStop();
    // 调整偏置
    int QT_BoardOffsetAdjust(double dbVoltageRangeCh1);

    // 频率控制字
    int phase_inc(double bitWidth, double ddsInputClockMHz, double ddsOutputFreqMHz, uint32_t *ctrlWord);
    // 相位控制字
    int phase_offset(double bitWidth, double ddsInputClockMHz, double ddsOutputFreqMHz, int ddsCount,
                     uint32_t *ctrlWord);
    int QT_BoardSetDacPhaseAndFreq(uint64_t freq, uint64_t phase_inc);
    int QT_BoardSetDaPlayStart(bool enable);
    int QT_BoardSetDacPlaySourceSelect(uint32_t sourceId);
    int QT_BoardSetDacPlayChannelSelect(uint32_t channelId);
    /**
     * @brief 循环读取
     * @param enable 循环读取使能 只在有限次传输模式下起作用,1=enable, 0=disable
     * @param count 循环读取次数(0=无限次循环，直至循环使能中止)
     **/
    int QT_BoardSetDACLoopRead(int enable, uint32_t count);
    // 设置DAC触发模式
    int QT_BoardSetDacTriggerMode(uint32_t triggermode);
    int QT_BoardSetDacRegister(uint32_t uWriteStartAddr, uint32_t uPlaySize);

    int QT_BoardSetOffset12136DC(int channel_id);
    // 获取数据
    // 函数功能: 获取数据
    // 函数参数：unOffsetAddr：起始地址 pBufDest：数据缓存区 unLen：数据长度 unTimeOut：超时时间
    // 函数返回: 成功返回0,非0表示失败
    int QT_BoardGetData(uint64_t unOffsetAddr, uint16_t *pBufDest, unsigned int unLen, unsigned int unTimeOut);
    std::vector<std::vector<uint16_t>> parseAndSplitData(const uint16_t *buffer, size_t bufferSize, size_t numChannels);

private:
    STXDMA_CARDINFO m_cardInfo;
    int Func_QTXdmaWriteRegister(STXDMA_CARDINFO *pstCardInfo, uint32_t uiBaseAddr, uint32_t uiRegOffset,
                                 uint32_t uiData);
};


#endif // QT_TEMPLATE_BOARDHARDWAREINTERFACE_H
