#pragma once
#include <QWidget>
#include "ui_dbinconf.h"

namespace Ui {
	class DBInConfWidget;
}

class DBInConfWidget :
	public QWidget
{
	Q_OBJECT
public:
	DBInConfWidget(QWidget * parent=NULL);
	~DBInConfWidget();
private:
	Ui::DBConf *ui;

public:
	QSize sizeHint() const;

public slots:
	void ckAllControls();
};

