#include "DBOutWidget.h"



DBOutWidget::DBOutWidget(QWidget * parent) :QWidget(parent), ui(new Ui::DBOut)
{
	ui->setupUi(this);
}


DBOutWidget::~DBOutWidget()
{
}

QSize DBOutWidget::sizeHint() const
{
    QRect rect = ui->formLayout->geometry();
    return QSize(rect.width(),rect.height());
}
