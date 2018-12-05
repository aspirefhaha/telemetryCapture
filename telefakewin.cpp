#include "telefakewin.h"
#include "ui_telefakewin.h"
#include "telecommuproto.h"
#include <QMessageBox>
#include "commonutils.h"

TeleFakeWin::TeleFakeWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TeleFakeWin),
    m_DataSocket(NULL),m_CmdSocket(NULL),m_StatSocket(NULL),
    m_saveStat(0),m_sendStat(0),m_lockStat(0),m_controlStat(0),
    statTimer(this),dataTimer(this),
    m_Roll(0),m_Yaw(0),m_Pitch(0)
{
    ui->setupUi(this);
    saveGroup.addButton(ui->rbStartSave);
    saveGroup.addButton(ui->rbStopSave);
    sendGroup.addButton(ui->rbStartSend);
    sendGroup.addButton(ui->rbStopSend);
    controlGroup.addButton(ui->rbLocalControl);
    controlGroup.addButton(ui->rbRemoteControl);
    ui->rbLocalControl->setChecked(true);
    ui->rbStopSave->setChecked(true);
    ui->rbStopSend->setChecked(true);

    connect(ui->pbConfirm,SIGNAL(clicked()),this,SLOT(slotConfirm()));
    connect(ui->pbReset,SIGNAL(clicked()),this,SLOT(slotReset()));
    connect(ui->pbLocalControl,SIGNAL(clicked()),this,SLOT(slotLocalControl()));
    connect(ui->pbRemoteControl,SIGNAL(clicked()),this,SLOT(slotRemoteControl()));
    connect(ui->pbStartSave,SIGNAL(clicked()),this,SLOT(slotStartSave()));
    connect(ui->pbStopSave,SIGNAL(clicked()),this,SLOT(slotStopSave()));
    connect(ui->pbStartSend,SIGNAL(clicked()),this,SLOT(slotStartSend()));
    connect(ui->pbStopSend,SIGNAL(clicked()),this,SLOT(slotStopSend()));
    connect(ui->pbLock,SIGNAL(clicked()),this,SLOT(slotLock()));
    connect(ui->pbUnlock,SIGNAL(clicked()),this,SLOT(slotUnlock()));
    connect(&statTimer,SIGNAL(timeout()),this,SLOT(slotSendStat()));
    connect(&dataTimer,SIGNAL(timeout()),this,SLOT(slotSendData()));

    connect(ui->vsYaw,SIGNAL(valueChanged(int)),this,SLOT(slotYawChanged(int)));
    connect(ui->vsPitch,SIGNAL(valueChanged(int)),this,SLOT(slotPitchChanged(int)));
    connect(ui->vsRoll,SIGNAL(valueChanged(int)),this,SLOT(slotRollChanged(int)));
}

TeleFakeWin::~TeleFakeWin()
{
    slotReset();
    if(m_DataSocket!=NULL){
        delete m_DataSocket;
        m_DataSocket = NULL;
    }
    if(m_StatSocket!=NULL){
        delete m_StatSocket;
        m_StatSocket = NULL;
    }
    if(m_CmdSocket!=NULL){
        delete m_CmdSocket;
        m_CmdSocket = NULL;
    }
    delete ui;
}

bool TeleFakeWin::initDataSocket()
{
    QStringList addraport = ui->leDataURL->text().split(":");
    if(addraport.size()<2){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的数据服务地址,如225.0.58.1:60058"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    m_dataPort = addraport.at(1).toUInt();

    if(m_dataPort == 0){
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
    }

    return true;
}

bool TeleFakeWin::initCmdSocket()
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
        connect(m_CmdSocket,SIGNAL(readyRead()),this,SLOT(processPendingCmdgrams()));
    }
    // qDebug() << "udp socket local port:"<< m_DataSocket->localPort();
    if(!m_CmdSocket->isValid() || m_CmdSocket->localPort() != m_cmdPort){
        bool bindret = m_CmdSocket->bind(QHostAddress::AnyIPv4,m_cmdPort,QAbstractSocket::ShareAddress);

        if(bindret){
            bindret = m_CmdSocket->joinMulticastGroup(m_cmdTarget);
            if(!bindret){
                QMessageBox::critical(NULL, "critical", ToQString("加入命令组播失败"), QMessageBox::Yes , QMessageBox::Yes);
                return false;
            }
        }
        else{
            QMessageBox::critical(NULL, "critical", ToQString("请先重置服务，或关闭已经打开的本程序的其他终端！"), QMessageBox::Yes , QMessageBox::Yes);
            return false;
        }
    }
    else{
        QMessageBox::critical(NULL, "critical", ToQString("请先重置服务"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    return true;
}

bool TeleFakeWin::initStatSocket()
{
    QStringList addraport = ui->leStatURL->text().split(":");
    if(addraport.size()<2){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的状态服务地址,,如225.0.58.2:60060"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    m_statPort = addraport.at(1).toUInt();
    if(m_statPort == 0){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的状态服务端口！"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    m_statTarget = QHostAddress(addraport.at(0));
    if(!m_statTarget.isMulticast()){
        QMessageBox::critical(NULL, "critical", ToQString("请输入正确的组播状态服务地址"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    if(m_StatSocket == NULL){
        m_StatSocket = new QUdpSocket(this);
    }
    return true;
}

void TeleFakeWin::slotConfirm()
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
    ui->pbLock->setEnabled(true);
    ui->pbUnlock->setEnabled(true);
    statTimer.start(TC_STATFREQ);
    int datafreq = ui->leDataFreq->text().toInt();
    if(datafreq <= 0){
        TC_DATAFREQ;
    }
    dataTimer.start(datafreq);
}

void TeleFakeWin::slotReset()
{
    statTimer.stop();
    dataTimer.stop();
    if(m_DataSocket!=NULL){
        if(m_DataSocket->isValid()){
            m_DataSocket->close();
            QThread::msleep(20);
        }
    }
    if(m_CmdSocket!=NULL){
        if(m_CmdSocket->isValid()){
            m_CmdSocket->leaveMulticastGroup(m_dataTarget);
            m_CmdSocket->disconnected();
            m_CmdSocket->close();
            QThread::msleep(20);
        }
    }
    if(m_StatSocket!=NULL){
        if(m_StatSocket->isValid()){
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
    ui->pbLock->setEnabled(false);
    ui->pbUnlock->setEnabled(false);
}

QSize TeleFakeWin::sizeHint() const
{
    QRect rect = ui->verticalLayout->geometry();
    return QSize(rect.width(),rect.height());
}

void TeleFakeWin::processPendingCmdgrams()
{
    quint64 datasize = m_CmdSocket->pendingDatagramSize();
    QByteArray datagram(datasize,Qt::Uninitialized);
    m_CmdSocket->readDatagram(datagram.data(),datagram.size());
    qDebug() << "Receice Cmd Data(" << datasize <<"):" << datagram.toHex();
    m_cmdBuffer+= datagram;
    do {
        while(m_cmdBuffer.size() > 0 && !m_cmdBuffer.startsWith(TC_HEAD1))m_cmdBuffer.remove(0,1);
        if(m_cmdBuffer.size() > 2){
            if((unsigned char)m_cmdBuffer.at(1) != TC_HEAD2){
                m_cmdBuffer.remove(0,2);
            }
            else if((unsigned long)m_cmdBuffer.size() >= sizeof(telecommu_cmd_proto)){   //包头符合要求且包长够一个完整协议
                if((unsigned char)m_cmdBuffer.at(2) == TC_TYPECMD){  //是命令帧
                    telecommu_cmd_proto * pCmdProto = (telecommu_cmd_proto *) m_cmdBuffer.data();
                    unsigned char calccrc;
                    CK_PACK_CRC(pCmdProto,calccrc);
                    if(calccrc == pCmdProto->crc){ //校验码正常
                        if(pCmdProto->cmd1 == TC_STARTSAVE1 && pCmdProto->cmd2 == TC_STARTSAVE2){
                            ui->rbStartSave->setChecked(true);
                        }
                        else if(pCmdProto->cmd1 == TC_STOPSAVE1 && pCmdProto->cmd2 == TC_STOPSAVE2){
                            ui->rbStopSave->setChecked(true);
                        }
                        else if(pCmdProto->cmd1 == TC_STARTSEND1 && pCmdProto->cmd2 == TC_STARTSEND2){
                            ui->rbStartSend->setChecked(true);
                        }
                        else if(pCmdProto->cmd1 == TC_STOPSEND1 && pCmdProto->cmd2 == TC_STOPSEND2){
                            ui->rbStopSend->setChecked(true);
                        }
                        else if(pCmdProto->cmd1 == TC_LOCALCONTROL1 && pCmdProto->cmd2 == TC_LOCALCONTROL2){
                            ui->rbLocalControl->setChecked(true);
                        }
                        else if(pCmdProto->cmd1 == TC_REMOTECONTROL1 && pCmdProto->cmd2 == TC_REMOTECONTROL2){
                            ui->rbRemoteControl->setChecked(true);
                        }
                        else{
                            qDebug() << "Get unknown cmd pack" << pCmdProto->cmd1 << " " << pCmdProto->cmd2;
                        }
                        m_cmdBuffer.remove(0,sizeof(telecommu_cmd_proto)); //处理完了11个字节
                    }
                    else {
                        qDebug() << "Get Cmd Pack err CRC " << QString::number(pCmdProto->crc,16) << " local calc crc:" << QString::number(calccrc,16);
                        m_cmdBuffer.remove(0,3);   //只扔掉3个字节
                    }
                }
                else{   //不是状态帧
                    m_cmdBuffer.remove(0,3);
                }
            }
        }
    }
    while((unsigned long)m_cmdBuffer.size() >= sizeof(telecommu_cmd_proto));
}


void TeleFakeWin::sendStatPack(const char * msg)
{
    ASSM_STAT_PACK(localControlPack,m_saveStat,m_sendStat,m_lockStat,m_controlStat);
    CALC_PACK_CRC(localControlPack);
    if (sizeof(localControlPack) != m_StatSocket->writeDatagram((const char *)&localControlPack,sizeof(localControlPack),m_statTarget,m_statPort)){
        QMessageBox::critical(NULL, "critical", ToQString(msg), QMessageBox::Yes , QMessageBox::Yes);
    }
}

void TeleFakeWin::sendDataPack(const char *msg)
{
    static bool sIsTwice = false;
    static unsigned char syncId = 0;
    unsigned char tmpbuf[TC_DATAPREFIXLEN];
    unsigned char composesubframe[TC_SUBTIMELEN+TC_SUBFRAMELEN];
    unsigned char * psubframe = &composesubframe[TC_SUBTIMELEN];
    memset(composesubframe,0,TC_SUBFRAMELEN+TC_SUBTIMELEN);
    if(!sIsTwice){
        if(TC_DATAPREFIXLEN != m_DataSocket->writeDatagram((const char *)tmpbuf,TC_DATAPREFIXLEN,m_dataTarget,m_dataPort)){
            qDebug() << msg;
        }
    }
    for(int i = 0 ;i<TC_SUBOFFULL;i++){
        QDateTime now = QDateTime::currentDateTime();
        QTime nowtime = now.time();
        QDate nowdate = now.date();
        unsigned int msec = nowtime.msec();
        composesubframe[0] = (((msec % 10) & 0xf) << 4) ;
        composesubframe[1] = (((msec  / 100) & 0xf) << 4) | (((msec % 100) / 10) & 0xf)  ;
        unsigned int sec = nowtime.second();
        composesubframe[2] = (((sec / 10) & 0xf) << 4) | ((sec % 10) & 0xf);
        //qDebug() << sec << " after" << QString::number(composesubframe[2],16);
        unsigned int min = nowtime.minute();
        composesubframe[3] = ((min / 10) & 0xf << 4) | ((min % 10) & 0xf);
        unsigned int hour = nowtime.hour();
        unsigned int day = nowdate.dayOfYear();
        composesubframe[4] = ((day % 10) << 6) | ((hour / 10) << 4) | ((hour % 10));
        composesubframe[5] = ((day / 100) << 6) | (((day / 10) % 10) << 2) | (day %10);
        *psubframe = syncId;
        syncId++;
        if(i==TC_SUBOFFULL-1){  //全帧结尾
            psubframe[TC_SUBFRAMELEN-2] = TC_FULLTAIL1;
            psubframe[TC_SUBFRAMELEN-1] = TC_FULLTAIL2;
        }
        else{   //子帧结尾
            psubframe[TC_SUBFRAMELEN-2] = TC_SUBTAIL1;
            psubframe[TC_SUBFRAMELEN-1] = TC_SUBTAIL2;
        }
        //开始填充副帧内容
        switch(i){
        case 0:
            //qDebug() << m_Yaw << " Hex " << QString::number(m_Yaw,16);
            composesubframe[TC_SUBTIMELEN + TC_VICEYAWOFFSET] = (( m_Yaw / 100) << 4) | ((m_Yaw / 10) % 10);
            //qDebug() << QString::number(composesubframe[TC_SUBTIMELEN + TC_VICEYAWOFFSET],16);
            composesubframe[TC_SUBTIMELEN + TC_VICEPITCHOFFSET] = (( m_Pitch / 100) << 4) | ((m_Pitch / 10) % 10);
            composesubframe[TC_SUBTIMELEN + TC_VICEROLLOFFSET] = (( m_Roll / 100) << 4) | ((m_Roll / 10) % 10);
            break;
        case 1:
            composesubframe[TC_SUBTIMELEN + TC_VICEYAWOFFSET] = (( m_Yaw % 10) << 4) ;
            //qDebug() << (m_Yaw % 100) << " left shift 4" <<  QString::number((m_Yaw % 100) << 4,16);
            //qDebug() << QString::number(composesubframe[TC_SUBTIMELEN + TC_VICEYAWOFFSET],16);
            composesubframe[TC_SUBTIMELEN + TC_VICEPITCHOFFSET] = (( m_Pitch % 10) << 4) ;
            composesubframe[TC_SUBTIMELEN + TC_VICEROLLOFFSET] = (( m_Roll % 10) << 4) ;
            break;
        case 2:
            break;
        case 3:
            break;
        }

        if(TC_SUBFRAMELEN + TC_SUBTIMELEN != m_DataSocket->writeDatagram((const char *)composesubframe,TC_SUBFRAMELEN+TC_SUBTIMELEN,m_dataTarget,m_dataPort)){
            qDebug() << msg;
        }
    }
    sIsTwice = !sIsTwice;
}

void TeleFakeWin::slotLocalControl()
{
    m_controlStat = 0x01;
}

void TeleFakeWin::slotRemoteControl()
{
    m_controlStat = 0x0;
}

void TeleFakeWin::slotLock()
{
    m_lockStat = 0x01;
}

void TeleFakeWin::slotUnlock()
{
    m_lockStat = 0x0;
}

void TeleFakeWin::slotStartSave()
{
    m_saveStat = 0x01;
}

void TeleFakeWin::slotStopSave()
{
    m_saveStat = 0x0;
}

void TeleFakeWin::slotStartSend()
{
    m_sendStat = 0x1;
}
void TeleFakeWin::slotStopSend()
{
    m_sendStat = 0x0;
}

bool TeleFakeWin::ckStatSocket()
{
    if(!m_StatSocket ){
        QMessageBox::critical(NULL, "critical", ToQString("状态发送通道异常"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    return true;
}

bool TeleFakeWin::ckDataSocket()
{
    if(!m_DataSocket ){
        QMessageBox::critical(NULL, "critical", ToQString("数据发送通道异常"), QMessageBox::Yes , QMessageBox::Yes);
        return false;
    }
    return true;
}

void TeleFakeWin::slotSendStat()
{
    if(!ckStatSocket()){
        return;
    }
    sendStatPack("发送状态包失败");
}

void TeleFakeWin::slotSendData()
{
    if(!ckDataSocket()){
        return;
    }
    sendDataPack("发射数据包失败！");
}

void TeleFakeWin::slotYawChanged(int nv)
{
    m_Yaw = nv;
    ui->lbYaw->setText(QString::number(nv));
}

void TeleFakeWin::slotPitchChanged(int nv)
{
    m_Pitch = nv;
    ui->lbPitch->setText(QString::number(nv));
}

void TeleFakeWin::slotRollChanged(int nv)
{
    m_Roll = nv;
    ui->lbRoll->setText(QString::number(nv));
}
