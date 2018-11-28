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
public:
	TelemetryInWidget(QWidget * parent);
	~TelemetryInWidget();
private:
	Ui::fTelemetryIn * ui;
    QButtonGroup * saveGroup;
    QButtonGroup * sendGroup;
    QButtonGroup * controlGroup;
public:
	QSize sizeHint() const;

public slots:
    void processPendingDatagrams();
    void slotConfirm();
    void slotReset();
    void slotStartSend();
    void slotStopSend();
    void slotStartSave();
    void slotStopSave();
    void slotLocalControl();
    void slotRemoteControl();
private:
    QUdpSocket * udpSocket;

};

