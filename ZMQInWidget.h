#pragma once
#include <QWidget>
#include "ui_zmqin.h"


namespace Ui {
	class ZMQInWidget;
}
class ZMQInWidget :
	public QWidget
{
    Q_OBJECT

public:
	ZMQInWidget(QWidget * parent);
	~ZMQInWidget();

private:
	Ui::ZMQInForm * ui;

public:
	QSize sizeHint() const;

public slots:
	void testSlot();
};

