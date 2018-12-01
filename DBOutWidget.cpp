#include "DBOutWidget.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "commonutils.h"


DBOutWidget::DBOutWidget(QWidget * parent) :QWidget(parent), ui(new Ui::DBOut), m_isDBOpened(false),m_savedItemCount(0)
{
    ui->setupUi(this);

    m_db = QSqlDatabase::addDatabase("QMYSQL");
    connect(ui->btnConfirm,SIGNAL(clicked()),this,SLOT(slotConfirm()));
    connect(ui->btnReset,SIGNAL(clicked()),this,SLOT(slotReset()));
    ui->btnReset->setEnabled(false);
}


DBOutWidget::~DBOutWidget()
{
}

QSize DBOutWidget::sizeHint() const
{
    QRect rect = ui->verticalLayout_3->geometry();
    return QSize(rect.width(),rect.height());
}

void DBOutWidget::slotConfirm()
{
    m_db.setHostName(ui->leServer->text());
    m_db.setPort(ui->lePort->text().toInt());
    m_db.setDatabaseName(ui->leDBName->text());       //这里输入你的数据库名
    m_db.setUserName(ui->leUser->text());
    m_db.setPassword(ui->lePwd->text());   //这里输入你的密码
    if (!m_db.open()) {
        QMessageBox::critical(0, QObject::tr("无法打开数据库"),"无法创建数据库连接！ ", QMessageBox::Cancel);
        return;
    }
    m_isDBOpened = true;
    ui->btnReset->setEnabled(true);
    ui->btnConfirm->setEnabled(false);
}

void DBOutWidget::slotReset()
{
    if(m_db.isOpen()){
        m_db.close();
    }
    m_isDBOpened = false;
    ui->btnConfirm->setEnabled(true);
    ui->btnReset->setEnabled(false);
}

void DBOutWidget::slotInsertTeleData(QByteArray ba)
{
    Q_UNUSED(ba);
    if(!m_isDBOpened){
        ui->lbStatus->setText("数据库未连接！！！！");
        return;
    }
    QDateTime recvDateTime = ParseDateTime(ba);

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO testdb(modelno,bindata)" "VALUES(:modelno,:bindata)");
    query.bindValue(":modelno","XT-1");
    query.bindValue(":bindata",ba);
    if(query.exec()){
        m_savedItemCount++;
        ui->lbStatus->setText(QString("保存 %1 条 %2").arg(m_savedItemCount).arg(recvDateTime.toString()));
    }
    else{
        ui->lbStatus->setText("数据库存储失败");
        qDebug() << query.lastError().text();
    }
}

bool DBOutWidget::isDBConnected()
{
    return m_isDBOpened;
}
