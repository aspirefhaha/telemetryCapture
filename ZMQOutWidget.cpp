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
    QRect rect = ui->verticalLayout->geometry();
    return QSize(rect.width(),rect.height());
}
