//
// Created by admin on 2024/9/6.
//

#include "BoardHardwareInterface.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <iostream>
#include "pub.h"


BoardHardwareInterface::BoardHardwareInterface() {
    // 初始化板卡信息
}

BoardHardwareInterface::~BoardHardwareInterface() {
    // 关闭板卡
    QT_BoardClose();
}

int BoardHardwareInterface::Func_QTXdmaWriteRegister(STXDMA_CARDINFO *m_cardInfo, uint32_t uiBaseAddr,
                                                     uint32_t uiRegOffset, uint32_t uiData) {
    if (!m_cardInfo) {

        return -1;
    }

    std::cout << "写入寄存器：" << pub::DecIntToHexStr(uiBaseAddr) << "+" << uiRegOffset << "="
              << pub::DecIntToHexStr(uiData) << std::endl;
    QTXdmaWriteRegister(m_cardInfo, uiBaseAddr, uiRegOffset, uiData);
    return 0;
}

// 获取卡信息
CardInfoData BoardHardwareInterface::QT_BoardGetCardInfo() {
    CardInfoData cardInfoData;
    uint64_t uintValue = 0;

    // 封装读寄存器的 lambda 函数，处理错误情况，返回 std::string
    auto readRegister = [&](uint64_t base, unsigned int offset) -> std::string {
        if (QTXdmaReadRegister(&m_cardInfo, base, offset, &uintValue) != 0) {
            qWarning() << "Failed to read register at base " << base << " offset " << offset;
            return ""; // 返回空字符串表示读取失败
        }
        return pub::DecIntToHexStr(uintValue); // 转换为十六进制字符串
    };
    // 读取卡类型
    cardInfoData.cardType = readRegister(BASE_BOARD_INFO, OFFSET_BDINFO_BDINFO);
    qDebug() << "Card Type is" << cardInfoData.cardType;

    // 读取卡逻辑版本
    cardInfoData.cardLogicalVersion = readRegister(BASE_BOARD_INFO, OFFSET_BDINFO_SOFT_VER);
    qDebug() << "Card Logical Version is" << cardInfoData.cardLogicalVersion;

    // 读取 ADC 信息
    std::string hexAdcStr = readRegister(BASE_BOARD_INFO, OFFSET_BDINFO_ADC);
    try {
        uintValue = std::stoi(hexAdcStr); // 将16进制字符串转换为无符号整数
    } catch (const std::invalid_argument &e) {
        qWarning() << "Failed to convert hex string to integer: " << e.what();
        return cardInfoData; // 返回已获取的数据，跳过后续操作
    } catch (const std::out_of_range &e) {
        qWarning() << "Hex string out of range: " << e.what();
        return cardInfoData;
    }


    // 计算采样率、通道数和分辨率
    cardInfoData.sampleRate = (uintValue / 10000); // 前两位
    cardInfoData.channelNumber = (uintValue % 10000) / 100; // 中间两位
    cardInfoData.resolution = uintValue % 100; // 最后两位


    qDebug() << "Card Sample Rate is" << cardInfoData.sampleRate;
    qDebug() << "Card Channel Number is" << cardInfoData.channelNumber;
    qDebug() << "Card Resolution is" << cardInfoData.resolution;

    // 每次读取卡信息后，重置通道信息数组
    cardInfoData.channels.clear();
    // 创建通道信息数组
    for (uint32_t i = 0; i < cardInfoData.channelNumber; ++i) {
        ChannelInfo channel{};
        channel.channelId = i;
        channel.offsetValue = 0; // 初始化偏置值为0
        cardInfoData.channels.push_back(channel); // 加入到通道数组
    }

    // 读取卡状态
    uint64_t currentState = 0;
    if (QTXdmaReadRegister(&m_cardInfo, BASE_BOARD_INFO, 0x20, &currentState) != 0) {
        qWarning() << "Failed to read card status.";
        return cardInfoData;
    }
    cardInfoData.cardStatus = currentState;
    qDebug() << "Card Status is" << QString::number(currentState, 16);
    return cardInfoData;
}


bool BoardHardwareInterface::QT_BoardOpen() {
    int iRet = QTXdmaOpenBoard(&m_cardInfo, 0);
    if (iRet == 0) {
        qDebug() << "板卡成功打开";
        return true;
    } else {
        qWarning() << "打开板卡失败，错误代码:" << iRet;
        return false;
    }
}

bool BoardHardwareInterface::QT_BoardClose() {
    int iRet = QTXdmaCloseBoard(&m_cardInfo);
    if (iRet == 0) {
        return true;
    }
    qWarning() << "关闭板卡失败，错误代码:" << iRet;
    return false;
}


int BoardHardwareInterface::QT_BoardInterruptClear() {
    int ret = 0;
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x04, 0);
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x04, 1);
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x04, 0);
    return ret;
}

int BoardHardwareInterface::QT_BoardADCStop() {
    int ret = 0;
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x10, 0);
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x10, 1);
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x10, 0);
    return ret;
}
int BoardHardwareInterface::QT_BoardReset() {
    int ret = 0;
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x18, 0);
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x18, 1);
    ret = BoardHardwareInterface::Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x18, 0);
    return ret;
}
int BoardHardwareInterface::QT_BoardSetChannelOffset(uint32_t channelId, uint32_t offsetValue) {
    // 定义常量
    const double reference_voltage = 2.5;
    const int scale_factor = 8192;

    // 寄存器地址表和常数表
    struct ChannelConfig {
        uint32_t offsetRegisterAddress;
        uint32_t triggerRegisterAddress;
        int adjustmentConstant;
    };

    // 通道配置数组
    ChannelConfig channels[] = {
            {0x20, 0x30, 169}, // 通道 1
            {0x24, 0x34, 175}, // 通道 2
            {0x28, 0x38, 157}, // 通道 3
            {0x2c, 0x3c, 195} // 通道 4
    };

    if (channelId < 1 || channelId > 4) {
        std::cerr << "Wrong channel id!" << std::endl;
        return -1; // 返回错误码
    }

    // 获取对应通道的配置
    const ChannelConfig &channel = channels[channelId - 1];

    // 计算偏置寄存器的值
    uint32_t single_offset_reg = static_cast<uint32_t>(
            (reference_voltage - offsetValue) * scale_factor / reference_voltage - channel.adjustmentConstant);

    // 写入寄存器
    int ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, channel.offsetRegisterAddress, single_offset_reg);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, channel.triggerRegisterAddress, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, channel.triggerRegisterAddress, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, channel.triggerRegisterAddress, 0);
    return ret;
}

int BoardHardwareInterface::QT_BoardSetPerTrigger(uint32_t pre_trig_length) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x28, pre_trig_length);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x2C, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x30, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x30, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x30, 0);
    return ret;
}
int BoardHardwareInterface::QT_BoardSetFrameheader(uint32_t frameheaderenable) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x38, frameheaderenable);
    return ret;
}
int BoardHardwareInterface::QT_BoardSetClockMode(uint32_t clockmode) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x04, clockmode);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x08, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x08, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x08, 0);
    Sleep(2000);

    uint64_t CurrentStates = 0;
    QTXdmaReadRegister(&m_cardInfo, BASE_BOARD_INFO, 0x20, &CurrentStates);
    printf("Card Status is %#x\n\n", CurrentStates);
    return ret;
}
int BoardHardwareInterface::QT_BoardMultiTrigerSingleDma(uint32_t DMAoncebytes, uint32_t once_trig_bytes,
                                                         uint32_t DAMTotolbytes) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x14, DMAoncebytes); // DMA单次搬运的长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x34, once_trig_bytes / 64 * 64); // 设置单次触发长度
    ret = Func_QTXdmaWriteRegister(
            &m_cardInfo, BASE_DMA_ADC, 0x18,
            DAMTotolbytes / 64 * 64); // 每一次触发的长度 如果是触发式采集应该注意触发频率和单次采集长度的大小关系
                                      // 单次采集长度<触发频率理论采集长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x1c,
                                   DAMTotolbytes / 64 * 64); // xdma传输段长(byte) 一般为触发次数* 单次触发长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x04, 0); // 乒乓操作第一块地址
    return ret;
}


int BoardHardwareInterface::QT_BoardSetStdSingleDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x14, 4 * 1024 * 1024); // DMA单次搬运的长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x34, once_trig_bytes / 64 * 64); // 设置单次触发长度
    ret = Func_QTXdmaWriteRegister(
            &m_cardInfo, BASE_DMA_ADC, 0x18,
            DMATotolbytes / 64 * 64); // 每一次触发的长度 如果是触发式采集应该注意触发频率和单次采集长度的大小关系
                                      // 单次采集长度<触发频率理论采集长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x1c,
                                   DMATotolbytes / 64 * 64); // xdma传输段长(byte) 一般为触发次数* 单次触发长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x04, 0); // 乒乓操作第一块地址
    return ret;
}

int BoardHardwareInterface::QT_BoardSetStdMultiDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x14, 4 * 1024 * 1024); // DMA单次搬运的长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x34, once_trig_bytes / 64 * 64); // 设置单次触发长度
    ret = Func_QTXdmaWriteRegister(
            &m_cardInfo, BASE_DMA_ADC, 0x18,
            DMATotolbytes / 64 * 64); // 每一次触发的长度 如果是触发式采集应该注意触发频率和单次采集长度的大小关系
                                      // 单次采集长度<触发频率理论采集长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x1c,
                                   DMATotolbytes / 64 * 64); // xdma传输段长(byte) 一般为触发次数* 单次触发长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x04, 0); // 乒乓操作第一块地址
    return ret;
}

int BoardHardwareInterface::QT_BoardSetFifoSingleDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x14, 4 * 1024 * 1024); // DMA单次搬运的长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x34, once_trig_bytes / 64 * 64); // 设置单次触发长度
    ret = Func_QTXdmaWriteRegister(
            &m_cardInfo, BASE_DMA_ADC, 0x18,
            DMATotolbytes / 64 * 64); // 每一次触发的长度 如果是触发式采集应该注意触发频率和单次采集长度的大小关系
                                      // 单次采集长度<触发频率理论采集长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x1c,
                                   DMATotolbytes / 64 * 64); // xdma传输段长(byte) 一般为触发次数* 单次触发长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR0_LOW, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR0_HIGH, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR1_LOW, 0x80000000);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR1_HIGH, 0);
    return ret;
}

int BoardHardwareInterface::QT_BoardSetFifoMultiDMAParameter(uint32_t once_trig_bytes, uint32_t DMATotolbytes) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x14, 4 * 1024 * 1024); // DMA单次搬运的长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x34, once_trig_bytes / 64 * 64); // 设置单次触发长度
    ret = Func_QTXdmaWriteRegister(
            &m_cardInfo, BASE_DMA_ADC, 0x18,
            DMATotolbytes / 64 * 64); // 每一次触发的长度 如果是触发式采集应该注意触发频率和单次采集长度的大小关系
                                      // 单次采集长度<触发频率理论采集长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x1c,
                                   DMATotolbytes / 64 * 64); // xdma传输段长(byte) 一般为触发次数* 单次触发长度
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR0_LOW, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR0_HIGH, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR1_LOW, 0x0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, OFFSET_DMA_ADC_BASEADDR1_HIGH, 0x1);
    return ret;
}


int BoardHardwareInterface::QT_BoardSetTransmitMode(int transmitpoints, int transmittimes) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x00, transmitpoints);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_ADC, 0x20, transmittimes);
    return ret;
}
int BoardHardwareInterface::QT_BoardSoftTrigger() {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x00, 0); // 设置触发模式
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x04, 1); // 设置触发次数
    return ret;
}
int BoardHardwareInterface::QT_BoardPulseTrigger(int counts, int pulse_period, int pulse_width) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x00, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x04, counts);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x08, pulse_period);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x0C, pulse_width);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x10, 1);
    return ret;
}
int BoardHardwareInterface::QT_BoardExtrigger_rising(int counts) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x00, 2); // 设置触发模式
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x04, counts); // 设置触发次数
    return ret;
}
int BoardHardwareInterface::QT_BoardExtrigger_falling(int counts) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x00, 3); // 设置触发模式
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x04, counts); // 设置触发次数
    return ret;
}
int BoardHardwareInterface::QT_BoardChannelTrigger(int mode, int counts, int channelID, int rasing_codevalue,
                                                   int falling_codevalue) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x00, mode);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x04, counts);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x24, channelID);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x14, rasing_codevalue);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x18, falling_codevalue);
    return ret;
}
int BoardHardwareInterface::QT_BoardSetInterruptSwitch() {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x00, 1);
    return ret;
}
int BoardHardwareInterface::QT_BoardSetADCStart() {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x08, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x08, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x08, 0);
    return ret;
}
void BoardHardwareInterface::QT_BoardInterruptGatherType() {
    uint32_t inpoint = 0;
    do {
        inpoint = QTXdmaGetOneEvent(&m_cardInfo);
        if (inpoint == 1) {
            QT_BoardInterruptClear();
        }
    } while (inpoint != 1);
}
int BoardHardwareInterface::QT_BoardSetADCStop() {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x10, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x10, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x10, 0);
    return ret;
}
void BoardHardwareInterface::QT_BoardPollingModeGatherType() {
    uint64_t uTriggerpoint = 0;
    do {
        QTXdmaReadRegister(&m_cardInfo, BASE_PCIE_INTR, 0x1C, &uTriggerpoint);
        if (uTriggerpoint == 1) {
            Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, OFFSET_PCIE_INTR_INTR_CLEAR,
                                     0); // 清除中断
            Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, OFFSET_PCIE_INTR_INTR_CLEAR,
                                     1); // 清除中断
            Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, OFFSET_PCIE_INTR_INTR_CLEAR,
                                     0); // 清除中断
        }
    } while (uTriggerpoint != 1);
}

int BoardHardwareInterface::QT_BoardOffsetAdjust(double dbVoltageRangeCh1) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x28,
                                   0xc26d); // 调整ADC模拟前端放大器的偏置
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x2C,
                                   0xc254); // 调整ADC模拟前端放大器的偏置
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x30,
                                   0xc274); // 调整ADC模拟前端放大器的偏置
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x34,
                                   0xc2a9); // 调整ADC模拟前端放大器的偏置

    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x38,
                                   0); // 下发放大器的偏置参数，用于设置量程
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x38, 1);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x38, 0);

    // 32’h0004_2108  (0.5倍) 270600
    // 32’h0004_A529  (1倍 ) 304425
    // 32’h000B_5AD6  (2倍 ) 744150
    // 32’h000B_DEF7  (4倍 ) 777975

    unsigned int temp = 304425;

    if (dbVoltageRangeCh1 == 0.5)
        temp = 777975;
    else if (dbVoltageRangeCh1 == 1)
        temp = 744150;
    else if (dbVoltageRangeCh1 == 2)
        temp = 304425;
    else if (dbVoltageRangeCh1 == 4)
        temp = 270600;

    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_ADC_CTRL, 0x24, temp); // 模拟端开关控制
    return ret;
}
int BoardHardwareInterface::phase_inc(double bitWidth, double ddsInputClockMHz, double ddsOutputFreqMHz,
                                      uint32_t *ctrlWord) {
    uint32_t *output = ctrlWord;
    // uint32_t *output2 = ctrlfreq;
    uint32_t PhaseInc = 0;
    uint32_t bitMask = 0;
    uint32_t Freq = 0;
    // 返回小数对整数部分的四舍五入值,比如 round(3.623); 返回4
    PhaseInc = (uint32_t) round(pow(2.0, bitWidth) / ddsInputClockMHz * ddsOutputFreqMHz);

    for (size_t i = 0; i < bitWidth; i++) {
        bitMask |= (1 << i);
    }
    // 取低bitWidth位的数据
    PhaseInc = PhaseInc & bitMask;
    //*output2 = Freq;
    *output = PhaseInc;
    return 0;
}
int BoardHardwareInterface::phase_offset(double bitWidth, double ddsInputClockMHz, double ddsOutputFreqMHz,
                                         int ddsCount, uint32_t *ctrlWord) {
    uint32_t PhaseOffset = 0;
    uint32_t bitMask = 0;
    uint32_t *output = ctrlWord;
    // 返回小数对整数部分的四舍五入值,比如 round(3.623); 返回4
    PhaseOffset = (uint32_t) round(pow(2.0, bitWidth) / ddsInputClockMHz * ddsOutputFreqMHz / ddsCount);
    for (size_t i = 0; i < bitWidth; i++) {
        bitMask |= (1 << i);
    }
    // 取低bitWidth位的数据
    PhaseOffset = PhaseOffset & bitMask;
    *output = PhaseOffset;
    return 0;
}
int BoardHardwareInterface::QT_BoardSetDacPhaseAndFreq(uint64_t freq, uint64_t phase_inc) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x14, freq); // DDS频率控制字
    ret = QTXdmaReadRegister(&m_cardInfo, BASE_DAC_CTRL, 0x14, &freq); // DDS频率控制字
    printf("%lld\n", freq);

    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x18, phase_inc); // DDS相位控制字
    ret = QTXdmaReadRegister(&m_cardInfo, BASE_DAC_CTRL, 0x18, &phase_inc); // DDS相位控制字
    printf("%lld\n", phase_inc);

    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x1C, 0);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x1C, 1); // 更新dds使能(上升沿有效)
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x1C, 0); // 更新dds使能(上升沿有效)
    return ret;
}
int BoardHardwareInterface::QT_BoardSetDaPlayStart(bool enable) {
    int ret = 0;
    if (enable) {
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x0C, 0);
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x0C, 1); // 启动上位机播放DA
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x0C, 0);
    } else {
        // DDS播放置为0
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x14, 0);
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x18, 1);

        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x1c, 0);
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x1c, 1);
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x1c, 0);

        // 上位机下发停止播放指令
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x14, 0x0);
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x14, 0x1); // 停止上位机播放
        Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x14, 0x0);
    }
    return ret;
}
int BoardHardwareInterface::QT_BoardSetDacPlaySourceSelect(uint32_t sourceId) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x20,
                                   sourceId); // 1 使能DA 播放数据文件 0使能DA播放DDS数据
    return ret;
}
int BoardHardwareInterface::QT_BoardSetDacPlayChannelSelect(uint32_t channelId) {
    int ret = 0;
    ret = QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL, 0x24, channelId); // 1 使能DA 播放数据文件 0使能DA播放DDS数据
    return ret;
}
int BoardHardwareInterface::QT_BoardSetDACLoopRead(int enable, uint32_t count) {
    int ret = 0;
    int eenable = enable ? 1 : 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_DAC, 0x1C, eenable);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_DAC, 0x20, count);
    return ret;
}

int BoardHardwareInterface::QT_BoardSetDacTriggerMode(uint32_t triggermode) {
    int ret = 0;
    Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DAC_CTRL2, 0x00, triggermode); // 触发模式设置
    return ret;
}
int BoardHardwareInterface::QT_BoardSetDacRegister(uint32_t uWriteStartAddr, uint32_t uPlaySize) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_DAC, 0x00,
                                   0); // 设置DMA传输模式为有限次传输模式
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_DAC, 0x04,
                                   uWriteStartAddr); // 设置DMA传数据地址
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_DAC, 0x14,
                                   4 * 1024 * 1024); // 设置DMA传输数据大小（单次数据量）
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_DMA_DAC, 0x18, uPlaySize); // 下发总数据量
    // ret = QT_BoardReg32Write(hCardInfo, BASE_DMA_DAC, 0x1c, 1); //使能DMA循环读取 ret = QT_BoardReg32Write(hCardInfo,
    // BASE_DMA_DAC, 0x20, 0);												//循环次数为无限次
    return 0;
}


uint32_t boardrange12136DC[4][1] = {{8086}, {8092}, {8084}, {8010}};
int BoardHardwareInterface::QT_BoardSetOffset12136DC(int channel_id) {
    int ret = 0;
    if (channel_id == 0) {
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x20, boardrange12136DC[channel_id][0]);
        // 使能信号下发
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x30, 0);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x30, 1);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x30, 0);

    } else if (channel_id == 1) {
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x24, boardrange12136DC[channel_id][0]);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x34, 0);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x34, 1);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x34, 0);
    } else if (channel_id == 2) {
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x28, boardrange12136DC[channel_id][0]);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x38, 0);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x38, 1);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x38, 0);
    } else if (channel_id == 3) {
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x2c, boardrange12136DC[channel_id][0]);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x3c, 0);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x3c, 1);
        ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_PCIE_INTR, 0x3c, 0);
    } else {
        // log info here
        puts("Wrong channel id!");
    }
    return ret;
}
int BoardHardwareInterface::QT_BoardExternalTrigger(int mode, int counts) {
    int ret = 0;
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x00, mode);
    ret = Func_QTXdmaWriteRegister(&m_cardInfo, BASE_TRIG_CTRL, 0x04, counts);
    return ret;
}
int BoardHardwareInterface::QT_BoardGetData(uint64_t unOffsetAddr, uint16_t *pBufDest, unsigned int unLen,
                                            unsigned int unTimeOut) {
    return QTXdmaGetDataBuffer(unOffsetAddr, &m_cardInfo, pBufDest, unLen, unTimeOut);
}


std::vector<std::vector<uint16_t>> BoardHardwareInterface::parseAndSplitData(const uint16_t *buffer, size_t bufferSize,
                                                                             size_t numChannels) {
    std::vector<std::vector<uint16_t>> channelData(numChannels);

    for (size_t i = 0; i < bufferSize; ++i) {
        size_t channelIndex = i % numChannels;
        channelData[channelIndex].push_back(buffer[i]);
    }

    // 输出每个通道的所有数据
    for (size_t i = 0; i < numChannels; ++i) {
        qDebug() << "通道" << i << "数据:";
        QString channelOutput;
        for (const auto &value: channelData[i]) {
            channelOutput += QString::number(value) + " ";

            // 每1000个数据换行一次，以提高可读性
            if (channelOutput.count(' ') % 1000 == 0) {
                qDebug().noquote() << channelOutput;
                channelOutput.clear();
            }
        }

        // 输出剩余的数据
        if (!channelOutput.isEmpty()) {
            qDebug().noquote() << channelOutput;
        }

        qDebug() << "通道" << i << "数据结束，总计" << channelData[i].size() << "个数据点";
    }

    return channelData;
}
