#include "TelemetryInWidget.h"
#include <QMessageBox>
#include "commonutils.h"


TelemetryInWidget::TelemetryInWidget(QWidget * parent) :QWidget(parent), ui(new Ui::fTelemetryIn)
{
	ui->setupUi(this);
    saveGroup = new QButtonGroup(this);
    sendGroup= new QButtonGroup(this);
    controlGroup= new QButtonGroup(this);
    saveGroup->addButton(ui->rbStartSave);
    saveGroup->addButton(ui->rbStopSave);
    sendGroup->addButton(ui->rbStartSend);
    sendGroup->addButton(ui->rbStopSend);
    controlGroup->addButton(ui->rbLocalControl);
    controlGroup->addButton(ui->rbRemoteControl);
    ui->rbLocalControl->setChecked(true);
    ui->rbStopSave->setChecked(true);
    ui->rbStopSend->setChecked(true);

    connect(ui->pbConfirm,SIGNAL(clicked(bool)),this,SLOT(slotConfirm()));
    connect(ui->pbReset,SIGNAL(clicked(bool)),this,SLOT(slotReset()));
}

TelemetryInWidget::~TelemetryInWidget()
{
}

void TelemetryInWidget::slotConfirm()
{
    if(udpSocket == NULL){
        udpSocket = new QUdpSocket(this);
        QStringList addraport = ui->leDataURL->text().split(":");
        if(addraport.size()<2){
            QMessageBox::critical(NULL, ToQString("输入错误"), ToQString("请输入正确的数据服务器地址"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        }
        udpSocket->bind(addraport.at(1).toInt());
        ui->pbReset->setEnabled(true);
        connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
        ui->pbConfirm->setEnabled(false);
    }
    else{
        QMessageBox::critical(NULL, ToQString("操作错误"), ToQString("请先重置服务"), QMessageBox::Yes , QMessageBox::Yes);
    }
}

void TelemetryInWidget::slotReset()
{

}

QSize TelemetryInWidget::sizeHint() const
{
    QRect rect = ui->verticalLayout->geometry();
    return QSize(rect.width(),rect.height());
}

void TelemetryInWidget::processPendingDatagrams()
{
    QByteArray datagram;
    datagram.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(datagram.data(),datagram.size());
    qDebug() << (tr("Receice Data: \"%1\"").arg(datagram.data()));
}
