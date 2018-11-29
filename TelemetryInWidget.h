#pragma once
#include <QWidget>
#include <QtNetwork>
#include "ui_telemetryin.h"

namespace Ui {
	class TelemetryInWidget;
}
class TelemetryInWidget :
	public QWidget
{
    Q_OBJECT
public:
	TelemetryInWidget(QWidget * parent);
	~TelemetryInWidget();
private:
	Ui::fTelemetryIn * ui;
    QButtonGroup saveGroup;
    QButtonGroup sendGroup;
    QButtonGroup controlGroup;
    QButtonGroup lockGroup;
public:
	QSize sizeHint() const;

public slots:
    void processPendingDatagrams();
    void processPendingStatgrams();
    void slotConfirm();
    void slotReset();
    void slotStartSend();
    void slotStopSend();
    void slotStartSave();
    void slotStopSave();
    void slotLocalControl();
    void slotRemoteControl();
signals:
    void udpReceived(QString);
private:
    QUdpSocket * m_DataSocket; //数据帧接收
    QUdpSocket * m_CmdSocket;   //命令帧发送
    QUdpSocket * m_StatSocket;  //状态接收帧
    QHostAddress m_cmdTarget;
    QHostAddress m_dataTarget;
    QHostAddress m_statTarget;
    quint32 m_cmdPort;

private:
    bool initDataSocket();
    bool initCmdSocket();
    bool initStatSocket();
    bool ckCmdSocket();
    void sendCmdPack(unsigned char c1,unsigned char c2,const char *  msg);
    QByteArray m_statBuffer;
    QByteArray m_dataBuffer;

};

