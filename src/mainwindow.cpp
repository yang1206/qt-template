//
// Created by admin on 2024/9/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <iostream>
#include "ui_mainwindow.h"


mainwindow::mainwindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);
    connect(ui->open_Board, &QPushButton::clicked, this, &mainwindow::openBoard);
    connect(ui->close_Board, &QPushButton::clicked, this, &mainwindow::closeBoard);
    setupUiSettings();
}

mainwindow::~mainwindow() { delete ui; }


void mainwindow::openBoard() {
    if (m_boardController.openBoard()) {
        statusBar()->showMessage("板卡成功打开");
        m_boardController.InterruptClear();
        m_boardController.ADCStop();
        m_boardController.Reset();
        m_boardController.SetTransmitMode(0, 0);

        showBoardInfo();

    } else {
        statusBar()->showMessage("板卡打开失败");
    }
}

void mainwindow::closeBoard() {
    m_boardController.closeBoard();
    m_boardController.InterruptClear();
    m_boardController.ADCStop();
    m_boardController.Reset();
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
    setChannel(cardInfo.channels);

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
void mainwindow::setChannel(std::vector<ChannelInfo> channels) {
    ui->tableWidgetChannel->clear();
    QStringList headerText;
    headerText << "通道" << "偏移值";
    ui->tableWidgetChannel->setColumnCount(headerText.size()); // 设置表格列数
    for (int i = 0; i < ui->tableWidgetChannel->columnCount(); i++) {
        QTableWidgetItem *headerItem = new QTableWidgetItem(headerText.at(i));
        QFont font = headerItem->font(); // 获取原有字体设置
        font.setBold(true); // 设置为粗体
        headerItem->setFont(font); // 设置字体
        ui->tableWidgetChannel->setHorizontalHeaderItem(i, headerItem); // 设置表头单元格的item
    }
    ui->tableWidgetChannel->setRowCount(channels.size()); // 设置表格的行数
    for (int i = 0; i < channels.size(); i++) {
        const ChannelInfo &channel = channels[i];
        QTableWidgetItem *channelNameItem = new QTableWidgetItem("通道" + QString::number(i + 1));
        channelNameItem->setFlags(channelNameItem->flags() & ~Qt::ItemIsEditable); // 设置为不可编辑
        channelNameItem->setFlags(channelNameItem->flags() | Qt::ItemIsUserCheckable); // 添加复选框
        channelNameItem->setCheckState(Qt::Checked); // 默认复选框选中
        ui->tableWidgetChannel->setItem(i, 0, channelNameItem);
        QTableWidgetItem *offsetValueItem = new QTableWidgetItem(QString::number(channel.offsetValue));
        offsetValueItem->setFlags(offsetValueItem->flags() & ~Qt::ItemIsEditable); // 设置为不可编辑
        ui->tableWidgetChannel->setItem(i, 1, offsetValueItem);
        ui->comboBoxTriggerChannel->addItem("通道" + QString::number(i + 1));
    }
}
void mainwindow::setupUiSettings() {
    // 触发模式
    QStringList triggerModes = {"软件触发",       "内部脉冲触发",   "外部脉冲上升沿触发", "外部脉冲下降沿触发",
                                "通道上升沿触发", "通道下降沿触发", "通道双沿触发",       "外部脉冲双沿触发"};
    ui->comboBoxTriggerMode->addItems(triggerModes);
    ui->comboBoxTriggerMode->setCurrentIndex(0);
    // 时钟模式
    QStringList clockModes = {"内参考时钟", "外采样时钟", "外参考时钟"};
    ui->comboBoxClockMode->addItems(clockModes);
    ui->comboBoxClockMode->setCurrentIndex(0);
    // 触发迟滞
    ui->spinBoxTriggerHysteresis->setValue(70);
    // 采样率
    ui->spinBoxsampleRate->setValue(1000);
    ui->spinBoxsampleRate->setSuffix(" MHz");
    // 预触发点数
    ui->spinBoxTriggerLength->setValue(0);
    // 触发次数
    ui->spinBoxTriggerCount->setValue(1);
    // 单次触发数据量
    ui->comboBoxOnceSamplebyte->addItems({"4096", "2048", "1024", "512", "256"});
    ui->comboBoxOnceSamplebyte->setCurrentText("2048"); // 8388608
    // 采集模式
    ui->comboBoxSampleMode->addItems({"无限点多次采集", "无限点单次采集", "有限点多次采集", "有限点单次采集"});
    ui->comboBoxSampleMode->setCurrentIndex(3);
    // 触发下降沿阈值
    ui->spinBoxTriggerFalling->setValue(-0);
}

int mainwindow::on_getData_Btn_clicked() {
    // 偏置调节
    for (int i = 0; i < 4; i++) {
        // 预触发设置
        int triggerLength = ui->spinBoxTriggerLength->value();
        m_boardController.m_boardInterface->QT_BoardSetPerTrigger(triggerLength);
    }

    // 帧头设置
    int frame = ui->checkBoxFrameHeader->isChecked() ? 1 : 0;
    m_boardController.m_boardInterface->QT_BoardSetFrameheader(frame);
    // 设置时钟模式
    int clockMode = ui->comboBoxClockMode->currentIndex();
    m_boardController.m_boardInterface->QT_BoardSetClockMode(clockMode);
    // 设置DMA参数
    int triggerCount = ui->spinBoxTriggerCount->value();
    int onceTriggerBytes = ui->comboBoxOnceSamplebyte->currentText().toInt();
    int DMATotolbytes = triggerCount * onceTriggerBytes;
    qDebug() << "正在进行DMA设置...";
    m_boardController.m_boardInterface->QT_BoardSetStdSingleDMAParameter(onceTriggerBytes, DMATotolbytes);

    int sampleMode = ui->comboBoxSampleMode->currentIndex();
    if (sampleMode == 0) {
        m_boardController.m_boardInterface->QT_BoardSetFifoMultiDMAParameter(onceTriggerBytes, DMATotolbytes);
        m_boardController.m_boardInterface->QT_BoardSetTransmitMode(1, 1);
    } else if (sampleMode == 1) {
        m_boardController.m_boardInterface->QT_BoardSetFifoSingleDMAParameter(onceTriggerBytes, DMATotolbytes);
        m_boardController.m_boardInterface->QT_BoardSetTransmitMode(1, 0);
    } else if (sampleMode == 2) {
        m_boardController.m_boardInterface->QT_BoardSetStdMultiDMAParameter(onceTriggerBytes, DMATotolbytes);
        m_boardController.m_boardInterface->QT_BoardSetTransmitMode(0, 1);
    } else if (sampleMode == 3) {
        m_boardController.m_boardInterface->QT_BoardSetStdSingleDMAParameter(onceTriggerBytes, DMATotolbytes);
        m_boardController.m_boardInterface->QT_BoardSetTransmitMode(0, 0);
    }

    // 触发模式
    int triggerMode = ui->comboBoxTriggerMode->currentIndex();
    uint32_t pulse_period = ui->spinBoxPulsePeriod->value();
    uint32_t pulse_width = 80;
    int triggerRising = ui->spinBoxTriggerRising->value();
    int triggerFalling = ui->spinBoxTriggerFalling->value();
    int triggerChannel = ui->comboBoxTriggerChannel->currentIndex();
    switch (triggerMode) {
        case 0:
            m_boardController.m_boardInterface->QT_BoardSoftTrigger();
            break;
        case 1:

            m_boardController.m_boardInterface->QT_BoardPulseTrigger(triggerCount, pulse_period, pulse_width);
            break;
        case 2:
            m_boardController.m_boardInterface->QT_BoardExtrigger_rising(triggerCount);
            break;
        case 3:
            m_boardController.m_boardInterface->QT_BoardExtrigger_falling(triggerCount);
            break;
        case 4:
            m_boardController.m_boardInterface->QT_BoardChannelTrigger(4, triggerCount, triggerChannel, triggerRising,
                                                                       triggerFalling);
            break;
        case 5:
            m_boardController.m_boardInterface->QT_BoardChannelTrigger(5, triggerCount, triggerChannel, triggerRising,
                                                                       triggerFalling);
            break;
        case 6: {
            m_boardController.m_boardInterface->QT_BoardChannelTrigger(6, triggerCount, triggerChannel, triggerRising,
                                                                       triggerFalling);
            break;
        }
        case 7:
            m_boardController.m_boardInterface->QT_BoardExternalTrigger(7, triggerCount);
            break;
        default:
            break;
    }

    // 使能PCIE中断
    m_boardController.m_boardInterface->QT_BoardSetInterruptSwitch();

    // 开始采集数据
    qDebug() << "开始采集数据...";
    m_boardController.m_boardInterface->QT_BoardSetADCStart();


    uint16_t aa = 0;
    uint32_t ADDR = 0;
    uint32_t offset = 0;
    uint32_t iReadbytes = 10 * 1024 * 1024;
    //    uint32_t iReadbytes = 512 * 1024;

    uint16_t *buffer = (uint16_t *) malloc(DMATotolbytes);

    if (NULL == buffer) {
        qCritical() << "内存分配失败!";
        return 0;
    }
    memset(buffer, 0, DMATotolbytes);

    m_boardController.m_boardInterface->QT_BoardInterruptGatherType();

    qDebug() << "数据采集完成，正在获取数据...";
    if (DMATotolbytes > iReadbytes) {
        do {
            if (DMATotolbytes >= iReadbytes) {
                int ret = m_boardController.m_boardInterface->QT_BoardGetData(ADDR + offset, &buffer[aa * iReadbytes],
                                                                              iReadbytes, 0);
                if (ret != 0) {
                    printf("获取数据失败！\n");
                    break;
                }
            } else if (DMATotolbytes < iReadbytes) {
                int ret1 = m_boardController.m_boardInterface->QT_BoardGetData(ADDR + offset, &buffer[aa * iReadbytes],
                                                                               DMATotolbytes, 0);
                if (ret1 != 0) {
                    printf("获取数据失败！\n");
                    break;
                }

                DMATotolbytes = 0;
                aa = 0;
                break;
            }

            DMATotolbytes -= iReadbytes;
            offset += iReadbytes;
            aa += 1;

        } while (DMATotolbytes > 0);
    } else {
        int ret2 =
                m_boardController.m_boardInterface->QT_BoardGetData(ADDR, buffer, DMATotolbytes, 0); // 获取数据到缓存
        if (ret2 != 0) {
            qDebug() << "获取数据失败！";
            free(buffer);
            return 0;
        }
    }

    // 假设每个样本是 uint16_t，计算样本数量
    size_t sampleCount = DMATotolbytes / sizeof(uint16_t);
    size_t channelCount = 4; // 假设有2个通道，根据实际情况调整

    // 调用新的解析和分割方法
    std::vector<std::vector<uint16_t>> splitData =
            m_boardController.m_boardInterface->parseAndSplitData(buffer, sampleCount, channelCount);
    // 释放内存
    free(buffer);
    buffer = nullptr;

    m_boardController.m_boardInterface->QT_BoardSetADCStop();
    return 0;
}
