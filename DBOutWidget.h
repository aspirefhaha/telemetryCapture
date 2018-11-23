#pragma once
#include <QWidget>
#include "ui_dbout.h"

namespace Ui {
	class DBOutWidget;
}
class DBOutWidget :
	public QWidget
{
public:
	DBOutWidget(QWidget * parent);
	~DBOutWidget();
private :
	Ui::DBOut * ui;
public:
	QSize sizeHint() const;
};

