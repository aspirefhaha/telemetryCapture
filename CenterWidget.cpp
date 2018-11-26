#include "CenterWidget.h"
#include <QMessageBox>
#include <QTime>
#include <QByteArray>

CenterWidget::CenterWidget(QWidget * parent):
    QWidget(parent),
    ui(new Ui::centerWidget)
{
    ui->setupUi(this);
    mockData();
}

CenterWidget::~CenterWidget()
{

}

void CenterWidget::mockData()
{
    //ui->listView->add
}

void CenterWidget::on_listWidget_itemClicked(QListWidgetItem *item)
{
    //QMessageBox::critical(NULL, "critical", item->text(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    QString dispstr = "";
    for ( int i = 0 ; i< 128 ;i++){
        int ranum1= qrand()% 16,ranum2 = qrand() % 16;
        dispstr.append(QString::number(ranum1,16) + QString::number(ranum2,16) + " ");
    }
    // QByteArray str2 = QByteArray::fromHex(dispstr.toLatin1().data());
    ui->textEdit->setText(item->text()  + "    "+ dispstr);
}
