#include "TelemetryInWidget.h"
#include <QMessageBox>
#include "commonutils.h"
#include "telecommuproto.h"


TelemetryInWidget::TelemetryInWidget(QWidget * parent) :QWidget(parent), ui(new Ui::fTelemetryIn),
    m_DataSocket(NULL),m_CmdSocket(NULL),m_StatSocket(NULL)
{
    ui->setupUi(this);
    saveGroup.addButton(ui->rbStartSave);
    saveGroup.addButton(ui->rbStopSave);
    sendGroup.addButton(ui->rbStartSend);
    sendGroup.addButton(ui->rbStopSend);
    controlGroup.addButton(ui->rbLocalControl);
    controlGroup.addButton(ui->rbRemoteControl);
    lockGroup.addButton(ui->rbDecLock);
    lockGroup.addButton(ui->rbDecUnlock);
    ui->rbLocalControl->setChecked(true);
    ui->rbStopSave->setChecked(true);
    ui->rbStopSend->setChecked(true);
    ui->rbDecUnlock->setChecked(true);

    connect(ui->pbConfirm,SIGNAL(clicked()),this,SLOT(slotConfirm()));
    connect(ui->pbReset,SIGNAL(clicked()),this,SLOT(slotReset()));
    connect(ui->pbLocalControl,SIGNAL(clicked()),this,SLOT(slotLocalControl()));
    connect(ui->pbRemoteControl,SIGNAL(clicked()),this,SLOT(slotRemoteControl()));
    connect(ui->pbStartSave,SIGNAL(clicked()),this,SLOT(slotStartSave()));
    connect(ui->pbStopSave,SIGNAL(clicked()),this,SLOT(slotStopSave()));
    connect(ui->pbStartSend,SIGNAL(clicked()),this,SLOT(slotStartSend()));
    connect(ui->pbStopSend,SIGNAL(clicked()),this,SLOT(slotStopSend()));
}

TelemetryInWidget::~TelemetryInWidget()
{
    slotReset();
    if(m_DataSocket!=NULL){
        delete m_DataSocket;
        m_DataSocket = NULL;
    }
}

bool TelemetryInWidget::initDataSocket()
{
    QStringList addraport = ui->leDataURL->text().split(":");
    if(addraport.size()<2){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的数据服务地址,如225.0.58.1:60058"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    quint32 bindport = addraport.at(1).toUInt();
    if(bindport == 0){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的数据服务端口"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    m_dataTarget = QHostAddress(addraport.at(0));
    if(!m_dataTarget.isMulticast()){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的组播数据服务地址"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    if(m_DataSocket == NULL){
        m_DataSocket = new QUdpSocket(this);
        connect(m_DataSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    }
    if(!m_DataSocket->isValid() || m_DataSocket->localPort() != bindport){
        bool bindret = m_DataSocket->bind(QHostAddress::AnyIPv4,bindport,QAbstractSocket::ShareAddress);

        if(bindret){
            bindret = m_DataSocket->joinMulticastGroup(QHostAddress(m_dataTarget));
            if(!bindret){
                QMessageBox::critical(NULL, "critical", ToQString("加入数据组播失败"), QMessageBox::Yes , QMessageBox::Yes);
                return false;
            }
        }
        else{
            QMessageBox::critical(NULL, "critical", ToQString("请先重置服务，或关闭已经打开的本程序的其他终端"), QMessageBox::Yes , QMessageBox::Yes);
            return false;
        }
    }
    else{
        QMessageBox::critical(NULL, "critical", ToQString("请先重置服务"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    return true;
}

bool TelemetryInWidget::initCmdSocket()
{
    QStringList addraport = ui->leCmdURL->text().split(":");
    if(addraport.size()<2){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的命令服务地址,如225.0.58.2:60059"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    m_cmdPort = addraport.at(1).toUInt();
    if(m_cmdPort == 0){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的命令服务端口"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    m_cmdTarget = QHostAddress(addraport.at(0));
    if(!m_cmdTarget.isMulticast()){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的组播命令服务地址"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    if(m_CmdSocket == NULL){
        m_CmdSocket = new QUdpSocket(this);
    }
    return true;
}

bool TelemetryInWidget::initStatSocket()
{
    QStringList addraport = ui->leStatURL->text().split(":");
    if(addraport.size()<2){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的状态服务地址,,如225.0.58.2:60060"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    quint32 bindport = addraport.at(1).toUInt();
    if(bindport == 0){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的状态服务端口"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    m_statTarget = QHostAddress(addraport.at(0));
    if(!m_statTarget.isMulticast()){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的组播状态服务地址"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    if(m_StatSocket == NULL){
        m_StatSocket = new QUdpSocket(this);
        connect(m_StatSocket,SIGNAL(readyRead()),this,SLOT(processPendingStatgrams()));
    }
    // qDebug() << "udp socket local port:"<< m_DataSocket->localPort();
    if(!m_StatSocket->isValid() || m_StatSocket->localPort() != bindport){
        bool bindret = m_StatSocket->bind(QHostAddress::AnyIPv4,bindport,QAbstractSocket::ShareAddress);

        if(bindret){
            bindret = m_StatSocket->joinMulticastGroup(QHostAddress(addraport.at(0)));
            if(!bindret){
                QMessageBox::critical(NULL, "critical", ToQString("加入状态组播失败"), QMessageBox::Yes , QMessageBox::Yes);
                return false;
            }
        }
        else{
            QMessageBox::critical(NULL, "critical", ToQString("请先重置服务，或关闭已经打开的本程序的其他终端"), QMessageBox::Yes , QMessageBox::Yes);
            return false;
        }
    }
    else{
        QMessageBox::critical(NULL, "critical", ToQString("请先重置服务"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    return true;
}

void TelemetryInWidget::slotConfirm()
{
    if(!initDataSocket()||!initCmdSocket()||!initStatSocket()){
        slotReset();
        return;
    }
    ui->pbReset->setEnabled(true);
    ui->pbConfirm->setEnabled(false);
    ui->pbStartSave->setEnabled(true);
    ui->pbStopSave->setEnabled(true);
    ui->pbStartSend->setEnabled(true);
    ui->pbStopSend->setEnabled(true);
    ui->pbLocalControl->setEnabled(true);
    ui->pbRemoteControl->setEnabled(true);
}

void TelemetryInWidget::slotReset()
{
    if(m_DataSocket!=NULL){
        if(m_DataSocket->isValid()){
            m_DataSocket->leaveMulticastGroup(m_dataTarget);
            m_DataSocket->disconnected();
            m_DataSocket->close();
            QThread::msleep(20);
        }
    }
    if(m_CmdSocket!=NULL){
        if(m_CmdSocket->isValid()){
            m_CmdSocket->close();
            QThread::msleep(20);
        }
    }
    if(m_StatSocket!=NULL){
        if(m_StatSocket->isValid()){
            m_StatSocket->leaveMulticastGroup(m_statTarget);
            m_StatSocket->disconnected();
            m_StatSocket->close();
            QThread::msleep(20);
        }
    }
    ui->pbConfirm->setEnabled(true);
    ui->pbReset->setEnabled(false);
    ui->pbStartSave->setEnabled(false);
    ui->pbStopSave->setEnabled(false);
    ui->pbStartSend->setEnabled(false);
    ui->pbStopSend->setEnabled(false);
    ui->pbLocalControl->setEnabled(false);
    ui->pbRemoteControl->setEnabled(false);
}

QSize TelemetryInWidget::sizeHint() const
{
    QRect rect = ui->verticalLayout->geometry();
    return QSize(rect.width(),rect.height());
}

void TelemetryInWidget::processPendingDatagrams()
{
    QByteArray datagram;
    quint64 datasize = m_DataSocket->pendingDatagramSize();
    qDebug() << "get udp data size: " << datasize;
    datagram.resize(datasize);
    m_DataSocket->readDatagram(datagram.data(),datagram.size());
    qDebug() << "Receice Data:" << datagram.toHex();
}

void TelemetryInWidget::processPendingStatgrams()
{
    QByteArray datagram;
    quint64 datasize = m_StatSocket->pendingDatagramSize();
    qDebug() << "get udp stat size: " << datasize;
    datagram.resize(datasize);
    m_StatSocket->readDatagram(datagram.data(),datagram.size());
    qDebug() << "Receice Stat Data:" << datagram.toHex();
    m_statBuffer+= datagram;
    do {
        while(m_statBuffer.size() > 0 && !m_statBuffer.startsWith(TC_HEAD1))m_statBuffer.remove(0,1);
        if(m_statBuffer.size() > 2){
            if(m_statBuffer.at(1) != TC_HEAD2){
                m_statBuffer.remove(0,2);
            }
            else if(m_statBuffer.size() > sizeof(telecommu_stat_proto)){   //包头符合要求且包长够一个完整协议
                if(m_statBuffer.at(2) == TC_TYPESTAT){  //是状态帧
                    telecommu_stat_proto * pStatProto = (telecommu_stat_proto *) m_statBuffer.data();
                    unsigned char calccrc;
                    CK_PACK_CRC(pStatProto,calccrc);
                    if(calccrc == pStatProto->crc){ //校验码正常
                        if(pStatProto->savestat==0x1){
                            ui->rbStartSave->setChecked(true);
                        }
                        else{
                            ui->rbStopSave->setChecked(true);
                        }
                        if(pStatProto->sendstat==0x1){
                            ui->rbStartSend->setChecked(true);
                        }
                        else{
                            ui->rbStartSend->setChecked(true);
                        }
                        if(pStatProto->lockstat==0x1){
                            ui->rbDecLock->setChecked(true);
                        }
                        else{
                            ui->rbDecUnlock->setChecked(true);
                        }
                        if(pStatProto->controlstat==0x1){
                            ui->rbLocalControl->setChecked(true);
                        }
                        else{
                            ui->rbRemoteControl->setChecked(true);
                        }
                        m_statBuffer.remove(0,sizeof(telecommu_stat_proto)); //处理完了11个字节
                    }
                    else {
                        m_statBuffer.remove(0,3);   //只扔掉3个字节
                    }
                }
                else{   //不是状态帧
                    m_statBuffer.remove(0,3);
                }
            }
        }
    }
    while(m_statBuffer.size() > sizeof(telecommu_cmd_proto));
}

void TelemetryInWidget::sendCmdPack(unsigned char c1,unsigned char c2,const char * msg)
{
    ASSM_CMD_PACK(localControlPack,c1,c2);
    CALC_PACK_CRC(localControlPack);
    if (sizeof(localControlPack) != m_CmdSocket->writeDatagram((const char *)&localControlPack,sizeof(localControlPack),m_cmdTarget,m_cmdPort)){
        QMessageBox::critical(NULL, "critical", ToQString(msg), QMessageBox::Yes , QMessageBox::Yes);
    }
}

void TelemetryInWidget::slotLocalControl()
{
    if(!ckCmdSocket())
        return;
    sendCmdPack(TC_LOCALCONTROL1,TC_LOCALCONTROL2,"发射本控失败");
}

void TelemetryInWidget::slotRemoteControl()
{
    if(!ckCmdSocket())
        return;
    sendCmdPack(TC_REMOTECONTROL1,TC_REMOTECONTROL2,"发射远控失败");
}

void TelemetryInWidget::slotStartSave()
{
    if(!ckCmdSocket())
        return;
    sendCmdPack(TC_STARTSAVE1,TC_STARTSAVE2,"发射开始保存失败");
}

void TelemetryInWidget::slotStopSave()
{
    if(!ckCmdSocket())
        return;
    sendCmdPack(TC_STOPSAVE1,TC_STOPSAVE2,"发射停止保存失败");
}

void TelemetryInWidget::slotStartSend()
{
    if(!ckCmdSocket())
        return;
    sendCmdPack(TC_STARTSEND1,TC_STARTSEND2,"发射开始发送失败");
}
void TelemetryInWidget::slotStopSend()
{
    if(!ckCmdSocket())
        return;
    sendCmdPack(TC_STOPSEND1,TC_STOPSEND2,"发射停止发送失败");
}

bool TelemetryInWidget::ckCmdSocket()
{
    if(!m_CmdSocket ){
        QMessageBox::critical(NULL, "critical", ToQString("命令发送通道异常"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    return true;
}
