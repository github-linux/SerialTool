#ifndef SIMPLESERIALTOOL_H
#define SIMPLESERIALTOOL_H

#include "qserialport.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SimpleSerialTool; }
QT_END_NAMESPACE

class QSerialPort;
class QTimer;

class SimpleSerialTool : public QMainWindow
{
    Q_OBJECT

public:
    SimpleSerialTool(QWidget *parent = nullptr);
    ~SimpleSerialTool();

private slots:
    void readSerialComData();

    void on_pushBtnOpen_clicked();

    void on_pushBtnClearRecv_clicked();

    void on_pushBtnSend_clicked();

private:
    void findSerialPort();

private:
    Ui::SimpleSerialTool *ui;

    QSerialPort* serial;
    QTimer* timer;

};
#endif // SIMPLESERIALTOOL_H
