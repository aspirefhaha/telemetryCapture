#ifndef TELEFAKEWIN_H
#define TELEFAKEWIN_H

#include <QMainWindow>
#include <QtNetwork>
#include <QButtonGroup>
namespace Ui {
class TeleFakeWin;
}

class TeleFakeWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit TeleFakeWin(QWidget *parent = 0);
    ~TeleFakeWin();

private:
    Ui::TeleFakeWin *ui;
    QButtonGroup saveGroup;
    QButtonGroup sendGroup;
    QButtonGroup controlGroup;
    QButtonGroup lockGroup;
public:
    QSize sizeHint() const;

public slots:
    void processPendingCmdgrams();
    void slotConfirm();
    void slotReset();
    void slotStartSend();
    void slotStopSend();
    void slotStartSave();
    void slotStopSave();
    void slotLocalControl();
    void slotRemoteControl();
    void slotLock();
    void slotUnlock();
    void slotSendStat();
    void slotSendData();
    void slotYawChanged(int);
    void slotPitchChanged(int);
    void slotRollChanged(int);

private:
    QUdpSocket * m_DataSocket; //数据帧接收
    QUdpSocket * m_CmdSocket;   //命令帧发送
    QUdpSocket * m_StatSocket;  //状态接收帧
    quint32 m_dataPort;
    QHostAddress m_dataTarget;
    QHostAddress m_statTarget;
    QHostAddress m_cmdTarget;
    quint32 m_statPort;
    quint32 m_cmdPort;
    unsigned char m_saveStat;
    unsigned char m_sendStat;
    unsigned char m_lockStat;
    unsigned char m_controlStat;

    QTimer statTimer;
    QTimer dataTimer;
    QByteArray m_cmdBuffer;

    int m_Roll;
    int m_Yaw ;
    int m_Pitch;

private:
    bool initDataSocket();
    bool initCmdSocket();
    bool initStatSocket();
    bool ckStatSocket();
    bool ckDataSocket();
    void sendStatPack(const char *  msg);
    void sendDataPack(const char *  msg);
};

#endif // TELEFAKEWIN_H
