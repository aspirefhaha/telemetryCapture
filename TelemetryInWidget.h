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
public:
	QSize sizeHint() const;

};

