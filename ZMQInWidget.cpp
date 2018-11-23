#include "ZMQInWidget.h"



ZMQInWidget::ZMQInWidget(QWidget * parent) :QWidget(parent), ui(new Ui::ZMQInForm)
{
	ui->setupUi(this);
}


ZMQInWidget::~ZMQInWidget()
{
}

QSize ZMQInWidget::sizeHint() const
{
	return QSize(320,125);
}

void ZMQInWidget::testSlot()
{
}
