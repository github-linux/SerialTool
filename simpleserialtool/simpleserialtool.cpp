#include "simpleserialtool.h"
#include "./ui_simpleserialtool.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>

SimpleSerialTool::SimpleSerialTool(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimpleSerialTool)
{
    ui->setupUi(this);
    this->serial = nullptr;

    this->findSerialPort();

    // 默认显示115200的波特率
    ui->comboBoxBaud->setCurrentIndex(7);

    // 每秒检测一下串口是否插拔
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &SimpleSerialTool::findSerialPort);
    this->timer->start(1000);

    ui->pushBtnOpen->setIcon(QIcon("../../0-image/play2.png"));
}

SimpleSerialTool::~SimpleSerialTool()
{
    delete ui;
}

void SimpleSerialTool::readSerialComData()
{
    // 读取串口数据
    QByteArray receiveData = this->serial->readAll();
    // 判断是否需要16进值显示
    if (ui->checkBoxHexRecv->isChecked()) {
        receiveData = receiveData.toHex();
    }
    QString receiveStr = QString(receiveData);
    // 显示接收到的数据
    ui->textEditRecv->insertPlainText(receiveStr);
}

void SimpleSerialTool::findSerialPort()
{
    static int lastPortCnt = 0;
    int portCnt = QSerialPortInfo::availablePorts().count();
    if (lastPortCnt == portCnt || this->serial != nullptr) {
        return;
    }
    lastPortCnt = portCnt;


    QString str;
    // 清除显示
    ui->comboBoxPort->clear();
    //foreach(const QSerialPortInfo &info; QSerialPortInfo::availablePorts)
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        str = info.portName() + ':' +  info.description();
        // 添加搜索到的端口信息
        ui->comboBoxPort->addItem(str);
    }
}


void SimpleSerialTool::on_pushBtnOpen_clicked()
{
    if (ui->pushBtnOpen->text() == QString("打开串口")) {
        this->serial = new QSerialPort;
        // 获取并设置端口号
        QString portInfo = ui->comboBoxPort->currentText();
        portInfo = portInfo.split(":").constFirst();
        this->serial->setPortName(portInfo);
        // 设置波特率
        this->serial->setBaudRate(ui->comboBoxBaud->currentText().toInt());
        // 设置停止位
        switch (ui->comboBoxStopBit->currentIndex()) {
        case 0: this->serial->setStopBits(QSerialPort::OneStop); break;
        case 1: this->serial->setStopBits(QSerialPort::OneAndHalfStop); break;
        case 2: this->serial->setStopBits(QSerialPort::TwoStop); break;
        default: break;
        }
        // 设置数据位
        switch(ui->comboBoxDataBit->currentText().toInt()) {
        case 8: this->serial->setDataBits(QSerialPort::Data8); break;
        case 7: this->serial->setDataBits(QSerialPort::Data7); break;
        case 6: this->serial->setDataBits(QSerialPort::Data6); break;
        case 5: this->serial->setDataBits(QSerialPort::Data5); break;
        default: break;
        }
        // 设置校验位
        switch(ui->comboBoxVerify->currentIndex()) {
        case 0: this->serial->setParity(QSerialPort::NoParity);
        case 1: this->serial->setParity(QSerialPort::OddParity);
        case 2: this->serial->setParity(QSerialPort::EvenParity);
        default: break;
        }
        // 设置流控
        this->serial->setFlowControl(QSerialPort::NoFlowControl);
        // 打开串口
        if (false == this->serial->open(QIODevice::ReadWrite)) {
            QMessageBox::warning(nullptr, "警告", "串口打开失败");
            delete this->serial;
            return;
        }

        // 设置信号槽
        connect(this->serial, &QSerialPort::readyRead, this, &SimpleSerialTool::readSerialComData);

        // 失能串口设置控件
        ui->comboBoxPort->setEnabled(false);
        ui->comboBoxBaud->setEnabled(false);
        ui->comboBoxStopBit->setEnabled(false);
        ui->comboBoxDataBit->setEnabled(false);
        ui->comboBoxVerify->setEnabled(false);
        //调整串口开关按钮显示的文字
        ui->pushBtnOpen->setText("关闭串口");
        ui->pushBtnOpen->setIcon(QIcon("../../0-image/stop.png"));
    }
    else {
        if (this->serial->isOpen()) {
            this->serial->close();
        }
        delete this->serial;
        this->serial = nullptr;

        //使能串口设置控件
        ui->comboBoxPort->setEnabled(true);
        ui->comboBoxBaud->setEnabled(true);
        ui->comboBoxStopBit->setEnabled(true);
        ui->comboBoxDataBit->setEnabled(true);
        ui->comboBoxVerify->setEnabled(true);
        //调整串口开关按钮显示的文字
        ui->pushBtnOpen->setText("打开串口");
        ui->pushBtnOpen->setIcon(QIcon("../../0-image/play2.png"));
    }
}



void SimpleSerialTool::on_pushBtnClearRecv_clicked()
{
    // 清空当前接收的内容
    ui->textEditRecv->clear();
}



void SimpleSerialTool::on_pushBtnSend_clicked()
{
    // 判断串口是否打开
    if (!this->serial || !this->serial->isOpen()) {
        QMessageBox::warning(nullptr, "警告", "请先打开串口");
        return;
    }

    // toPlainText()    转换为纯文本
    // toUtf8()         转换为UTF-8编码
    QByteArray sendData = ui->textEditSend->toPlainText().toUtf8();

    // 是否勾选16进制发送
    if (ui->checkBoxHexSend->isChecked()) {
        int sendCnt = sendData.size();
        char *data = sendData.data();

        for (int i=0; i<sendCnt; i++) {
            //判断发送字符是否非法
            if ((data[i]>='0') && (data[i]<='9')) {
                continue;
            }
            else if ((data[i]<='f') && (data[i]>='a')) {
               continue;
            }
            else if ((data[i]>='A') && (data[i]<='F')) {
                continue;
            }
            else if (data[i] == ' ') {
                continue;
            }
            else {
                QMessageBox::warning(NULL, "提示", "存在非16进制字符!");
                return;
            }
        }

       sendData = sendData.fromHex(sendData);
    }

    //是否勾选新行发送
    if (ui->checkBoxNewLine->isChecked()) {
        int sendCnt = sendData.size();
        sendData = sendData.insert(sendCnt, "\r\n");
    }

    this->serial->write(sendData);
}

