#include "ZMQOutWidget.h"

ZMQOutWidget::ZMQOutWidget(QWidget * parent):QWidget(parent), ui(new Ui::fZMQOut)
{
	ui->setupUi(this);
}

ZMQOutWidget::~ZMQOutWidget()
{
}

QSize ZMQOutWidget::sizeHint() const
{
    return QSize(260,117);
}
