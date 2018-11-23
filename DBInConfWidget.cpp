#include "DBInConfWidget.h"



DBInConfWidget::DBInConfWidget(QWidget * parent):QWidget(parent), ui(new Ui::DBConf)
{
	ui->setupUi(this);
}



DBInConfWidget::~DBInConfWidget()
{
}

QSize DBInConfWidget::sizeHint() const
{
	return QSize(320, 240);
}

void DBInConfWidget::ckAllControls()
{
	if (ui->cbEnabled->isChecked()) {

		printf("here");
	}
	else {
		printf("there");

	}
}
