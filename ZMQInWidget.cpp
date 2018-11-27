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
    QRect rect = ui->verticalLayout->geometry();
    return QSize(rect.width(),rect.height());
}

void ZMQInWidget::testSlot()
{
}
