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
    connect(this,SIGNAL(udpReceived(QString)),parent,SLOT(status(QString)));
}

TelemetryInWidget::~TelemetryInWidget()
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
            bindret = m_StatSocket->joinMulticastGroup(m_statTarget);
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
    quint64 datasize = m_DataSocket->pendingDatagramSize();
    QByteArray datagram(datasize,Qt::Uninitialized);
    m_DataSocket->readDatagram(datagram.data(),datagram.size());
    // qDebug() << "Receice Data(" << datasize << "):" << datagram.toHex();
    m_dataBuffer += datagram;
//    qDebug() << "start datasize " << m_dataBuffer.size();
    if(m_dataBuffer.size() >= TC_SUBFRAMELEN + TC_SUBTIMELEN){
        int buflen = m_dataBuffer.size();
        bool findflag = false;  //用于判断本轮是否是要删除前面没有用的数据，当是false的时候，需要删除长度大于TC_SUBFRAMELEN + TC_SUBTIMELEN的前面所有数据数据
        for(int i= 0 ;i < buflen ;i++){
            if((unsigned char)m_dataBuffer.at(i) == TC_SUBTAIL1){ //找到子帧结尾第一个字节
//                qDebug() << "find subframe tail1 at" << i;
                if( i == buflen - 1){ // 到结尾了，少了一个TC_SUBTAIL2
                    findflag = true;
                    //去掉前面多余的无用字节
                    int cutlen = buflen - (TC_SUBFRAMELEN + TC_SUBTIMELEN -1 );
                    if(cutlen > 0){
                        m_dataBuffer.remove(0,cutlen);
                    }
                    break;
                }
                else if((unsigned char ) m_dataBuffer.at(i+1) == TC_SUBTAIL2){ // 找到了子帧的帧尾
//                    qDebug() << "find subframe tail2 at" << i+1;
                    if(i < TC_SUBFRAMELEN + TC_SUBTIMELEN -1){ //本子帧不全或者不是帧结尾

                    }
                    else{   //一个完整子帧可以处理了,不过有可能不是有效的子帧，可能正好在数据中存在两个字节和子帧的帧尾相同
                        findflag = true;
                        // qDebug() << "find one subframe";
                        m_frames.append(m_dataBuffer.mid(i-TC_SUBFRAMELEN - TC_SUBTIMELEN + 2,TC_SUBFRAMELEN+TC_SUBTIMELEN));
                        i++;
                        //m_dataBuffer.remove(0,i+1);
                    }
                }
            }
            else if((unsigned char)m_dataBuffer.at(i) == TC_FULLTAIL1){ //找到全帧结尾第一个字节
                if( i == buflen - 1){ // 到结尾了，少了一个TC_FULLTAIL2
                    findflag = true;
                    //去掉前面多余的无用字节
                    int cutlen = buflen - (TC_SUBFRAMELEN + TC_SUBTIMELEN -1 );
                    if(cutlen > 0){
                        m_dataBuffer.remove(0,cutlen);
                    }
                    break;
                }
                else if((unsigned char ) m_dataBuffer.at(i+1) == TC_FULLTAIL2){ // 找到了全帧的帧尾
                    if(i < TC_SUBFRAMELEN + TC_SUBTIMELEN -1){ //本子帧不全

                    }
                    else{   //一个完整子帧可以处理了,不过有可能不是有效的子帧，可能正好在数据中存在两个字节和子帧的帧尾相同
                        findflag = true;
//                        qDebug() << "find on fullframe";
                        m_frames.append(m_dataBuffer.mid(i-TC_SUBFRAMELEN - TC_SUBTIMELEN + 2,TC_SUBFRAMELEN+TC_SUBTIMELEN));
                        i++;
                        //m_dataBuffer.remove(0,i+1);
                    }
                }
            }
        }//一轮寻找结束
        if(m_dataBuffer.size() >= TC_SUBFRAMELEN + TC_SUBTIMELEN){
            m_dataBuffer.remove(0,m_dataBuffer.size() - (TC_SUBTIMELEN + TC_SUBFRAMELEN-1));
        }
    }
//    qDebug() << "in " << m_subframecount << " size " << m_dataBuffer.size() ;
    if(m_frames.size() >= TC_SUBOFFULL){
        processFullFrame();
    }
}

void TelemetryInWidget::processFullFrame()
{
    if(m_frames.size() <= 0){
        return;
    }
    //qDebug() << "before process vec size " << m_frames.size() ;

    bool findFullTail = false;
    char syncId = 0;
    char nextSyncId = 0;
    QVector<QByteArray> findFullFrames;
    QByteArray curFullFrames;
    QVector<int> delIndexs;
    for(int i = m_frames.size() -1 ;i>=0;i--){
        const QByteArray &frame = m_frames.at(i);
        //找到全帧的结尾
        unsigned char tail1 = (unsigned char )frame.at(TC_SUBFRAMELEN+TC_SUBTIMELEN-2);
        unsigned char tail2 = (unsigned char )frame.at(TC_SUBFRAMELEN+TC_SUBTIMELEN-1);
        //qDebug() << frame.toHex();
        //qDebug() << QString::number(tail1 ,16) << " " << QString::number(tail2,16);
        if( tail1 == TC_FULLTAIL1 && tail2 == TC_FULLTAIL2){
            // qDebug() << "find tail frame";
            if(findFullTail){
                //两个全帧结尾之间没有足够的子帧
                curFullFrames.clear();
            }
            findFullTail = true;
            syncId = frame.at(TC_SUBTIMELEN);
            nextSyncId = syncId - 1;
            delIndexs.append(i);
            curFullFrames.prepend(frame);
        }
        else if(tail1 == TC_SUBTAIL1 && tail2 == TC_SUBTAIL2){
            //找到了子帧
            if(findFullTail){
                //已经找到了全帧尾帧
                delIndexs.append(i);
                syncId = frame.at(TC_SUBTIMELEN);
                if(syncId == nextSyncId){ //子帧的syncId对应没有问题
                    // qDebug() << " find sub frame";
                    nextSyncId--;
                    curFullFrames.prepend(frame);
                }
                else{
                    //错误的子帧
                    qDebug() << "bad sync need " << nextSyncId << " but get " << syncId;
                }
                if(curFullFrames.size() == (TC_SUBFRAMELEN + TC_SUBTIMELEN) * TC_SUBOFFULL ){
                    //一个全帧ok了
                    findFullFrames.prepend(curFullFrames);
                    findFullTail = false;

                }
            }
            else{ //没有找到过对应的尾帧
                //暂时什么都不用做,等待下次处理
            }
        }
        else{
            //错误的同步字，删除帧
            delIndexs.append(i);
        }
    }

    //删除没有用的帧，包括帧已经过多，需要删除
    for(QVector<int>::iterator iiter=delIndexs.begin();iiter != delIndexs.end();iiter++){
        m_frames.remove(*iiter,1);
    }
    if(m_frames.size() >= 3* TC_SUBOFFULL){
        m_frames.remove(0,TC_SUBOFFULL);
    }
    // qDebug() << "find good full frame " << findFullFrames.size();
    for(QVector<QByteArray>::iterator vaiter = findFullFrames.begin();vaiter!=findFullFrames.end();vaiter++){
        emit getFullFrame(*vaiter);
    }


}

void TelemetryInWidget::processPendingStatgrams()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    int datasize = (int)m_StatSocket->pendingDatagramSize();
    QByteArray datagram(datasize,Qt::Uninitialized);
    m_StatSocket->readDatagram(datagram.data(),datagram.size());
    QString debugmsg =QString("%1 Receive Stat Data(%2): %3")
            .arg(current_date_time.toString("hh:mm:ss zzz"))
            .arg(datasize)
            .arg(QString(datagram.toHex())) ;
    // qDebug()<< debugmsg;
    emit udpReceived(debugmsg);
    m_statBuffer +=  datagram;
    do {
        while(m_statBuffer.size() > 0 && !m_statBuffer.startsWith(TC_HEAD1))m_statBuffer.remove(0,1);
        if(m_statBuffer.size() > 2){
            if((unsigned char)m_statBuffer.at(1) != TC_HEAD2){
                m_statBuffer.remove(0,2);
            }
            else if(m_statBuffer.size() >= (int)sizeof(telecommu_stat_proto)){   //包头符合要求且包长够一个完整协议
                if(m_statBuffer.at(2) == TC_TYPESTAT){  //是状态帧
                    telecommu_stat_proto * pStatProto = (telecommu_stat_proto *) m_statBuffer.data();
                    unsigned char calccrc = 0;
                    CK_PACK_CRC(pStatProto,calccrc);
//                    do{
//                        unsigned char * a = (unsigned char *)pStatProto;
//                        calccrc = 0;
//                        for(unsigned i = 2; i<sizeof(*pStatProto)-1;i++){
//                            calccrc += a[i];
//                            qDebug() << QString::number(calccrc,16) << " " << QString::number(a[i],16);
//                        }
//                    }while(0);
                    if(calccrc == pStatProto->crc){ //校验码正常
                        if(pStatProto->savestat==0x1){
                            ui->rbStartSave->setChecked(true);
                        }
                        else if (pStatProto->savestat == 0){
                            ui->rbStopSave->setChecked(true);
                        }
                        else{
                            qDebug() << "unknown save stat:" << pStatProto->savestat;
                        }
                        if(pStatProto->sendstat==0x1){
                            ui->rbStartSend->setChecked(true);
                        }
                        else if(pStatProto->sendstat==0){
                            ui->rbStopSend->setChecked(true);
                        }
                        else{
                            qDebug() << "unknown send stat:" << pStatProto->sendstat;
                        }
                        if(pStatProto->lockstat==0x1){
                            ui->rbDecLock->setChecked(true);
                        }
                        else if(pStatProto->lockstat==0){
                            ui->rbDecUnlock->setChecked(true);
                        }
                        else{
                            qDebug() << "unknown lock stat:" << pStatProto->lockstat;
                        }
                        if(pStatProto->controlstat==0x1){
                            ui->rbLocalControl->setChecked(true);
                        }
                        else if(pStatProto->controlstat==0){
                            ui->rbRemoteControl->setChecked(true);
                        }
                        else{
                            qDebug() << "unknown control stat:" << pStatProto->controlstat;
                        }
                        m_statBuffer.remove(0,sizeof(telecommu_stat_proto)); //处理完了11个字节
                    }
                    else {
                        qDebug() << "Get Stat Pack err CRC " << QString::number(pStatProto->crc,16) << " local calc crc:" << QString::number(calccrc,16);
                        m_statBuffer.remove(0,3);   //只扔掉3个字节
                    }
                }
                else{   //不是状态帧
                    m_statBuffer.remove(0,3);
                }
            }
        }
    }
    while(m_statBuffer.size() >= (int)sizeof(telecommu_stat_proto));
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
