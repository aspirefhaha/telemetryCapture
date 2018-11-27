#include "TelemetryInWidget.h"


TelemetryInWidget::TelemetryInWidget(QWidget * parent) :QWidget(parent), ui(new Ui::fTelemetryIn)
{
	ui->setupUi(this);
    saveGroup = new QButtonGroup(this);
    sendGroup= new QButtonGroup(this);
    controlGroup= new QButtonGroup(this);
    saveGroup->addButton(ui->rbStartSave);
    saveGroup->addButton(ui->rbStopSave);
    sendGroup->addButton(ui->rbStartSend);
    sendGroup->addButton(ui->rbStopSend);
    controlGroup->addButton(ui->rbLocalControl);
    controlGroup->addButton(ui->rbRemoteControl);
    ui->rbLocalControl->setChecked(true);
    ui->rbStopSave->setChecked(true);
    ui->rbStopSend->setChecked(true);
}

TelemetryInWidget::~TelemetryInWidget()
{
}

QSize TelemetryInWidget::sizeHint() const
{
    QRect rect = ui->verticalLayout->geometry();
    return QSize(rect.width(),rect.height());
}
