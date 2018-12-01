#pragma once
#include <QWidget>
#include "ui_zmqout.h"

namespace Ui {
	class ZMQOutWidget;
}
class ZMQOutWidget :
	public QWidget
{
    Q_OBJECT
public:
	ZMQOutWidget(QWidget * parent);
	~ZMQOutWidget();
private:
	Ui::fZMQOut * ui;

public:
	QSize sizeHint() const;
};

