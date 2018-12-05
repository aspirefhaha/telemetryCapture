#include "CenterWidget.h"
#include <QMessageBox>
#include <QTime>
#include <QByteArray>
#include <QVariant>
#include "chartWidget.h"
#include "barwidget.h"
#include "commonutils.h"
#include <QDebug>

CenterWidget::CenterWidget(QWidget * parent):
    QWidget(parent),
    ui(new Ui::centerWidget)
{
    ui->setupUi(this);
    ui->gridLayout->removeWidget(ui->tabWidget);
    ui->gridLayout->addWidget(ui->tabWidget,1,1,2,1);
    mockData();
    chartwidget = new chartWidget(this);
    barwidget = new barWidget(this);
    ui->tabWidget->insertTab(0,chartwidget,"Chart");
    ui->tabWidget->insertTab(1,barwidget,"Bar");
    ui->tabWidget->setCurrentIndex(0);
    //ui->openGLWidget = new GLWidget(size().width(),size().height());
    m_pGLWidget = new SpaceTrekGLWidget(100,100,this);
    ui->gridLayout->addWidget(m_pGLWidget,0,0);

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(on_centerTabChanged(int)));
    connect(ui->pbClearList,SIGNAL(clicked()),this,SLOT(clearDataList()));
}

CenterWidget::~CenterWidget()
{

}

void CenterWidget::clearDataList()
{
    ui->listWidget->clear();
}


void CenterWidget::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    //m_pGLWidget->keyPressEvent(event);
    qDebug() << "keyPressEvent";
}

void CenterWidget::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
    //m_pGLWidget->wheelEvent(event);
    qDebug() << "wheelEvent";
}

//void CenterWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    if(event->buttons() & Qt::RightButton)
//    {
//        qDebug() << "right Button";
//    }
//}

void CenterWidget::mockData()
{
    //ui->listView->add
    ui->listWidget->clear();
}

void CenterWidget:: on_centerTabChanged(int idx)
{
    // qDebug() << idx;
    if(idx==1){
        barwidget->updateBarData();
    }
}

void CenterWidget::recvData(QByteArray ba)
{
    QDateTime recvDateTime = ParseDateTime(ba);
    QListWidgetItem * item = new QListWidgetItem;
    item->setData(Qt::DisplayRole, recvDateTime.toString());
    item->setData(Qt::UserRole,ba);
    ui->listWidget->insertItem(0,item);
    if(ui->listWidget->count()>=100){
        ui->listWidget->removeItemWidget(ui->listWidget->item(100));
        //ui->listWidget->clear();
    }
}

void CenterWidget::postureChanged(qreal yaw, qreal pitch, qreal roll)
{
    m_pGLWidget->postureChanged(yaw,pitch,roll);
    qreal rawdata[3] = {yaw,pitch,roll};
    chartwidget->updateRawData((qreal *)rawdata);
}

void CenterWidget::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QByteArray ba = item->data(Qt::UserRole).toByteArray();
    ui->textEdit->setText(item->text()  + "  "+  ba.toHex());
}
