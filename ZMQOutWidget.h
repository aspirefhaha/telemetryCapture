#pragma once
#include <QWidget>
#include "ui_zmqout.h"

namespace Ui {
	class ZMQOutWidget;
}
class ZMQOutWidget :
	public QWidget
{
public:
	ZMQOutWidget(QWidget * parent);
	~ZMQOutWidget();
private:
	Ui::fZMQOut * ui;

public:
	QSize sizeHint() const;
};

