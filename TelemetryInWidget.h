#pragma once
#include <QWidget>
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

};

