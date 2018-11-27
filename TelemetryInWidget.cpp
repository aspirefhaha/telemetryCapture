#include "TelemetryInWidget.h"


TelemetryInWidget::TelemetryInWidget(QWidget * parent) :QWidget(parent), ui(new Ui::fTelemetryIn)
{
	ui->setupUi(this);
}

TelemetryInWidget::~TelemetryInWidget()
{
}

QSize TelemetryInWidget::sizeHint() const
{
    return QSize(260,113);
}
