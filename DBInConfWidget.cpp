#include "DBInConfWidget.h"
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "telecommuproto.h"

DBInConfWidget::DBInConfWidget(QWidget * parent):QWidget(parent), ui(new Ui::DBConf)
{
    ui->setupUi(this);

    connect(ui->btnConfirm,SIGNAL(clicked()),this,SLOT(slotConfirm()));
    connect(ui->btnReset,SIGNAL(clicked()),this,SLOT(slotReset()));
    connect(ui->pbQuery,SIGNAL(clicked()),this,SLOT(slotQuery()));

    ui->btnReset->setEnabled(false);
}

DBInConfWidget::~DBInConfWidget()
{
}

QSize DBInConfWidget::sizeHint() const
{

    return ui->verticalLayout->sizeHint();
}

void DBInConfWidget::ckAllControls()
{

}

void DBInConfWidget::slotConfirm()
{
    if(m_db.isOpen()){
        m_db.close();
    }
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    else
        m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(ui->leServer->text());
    m_db.setPort(ui->lePort->text().toInt());
    m_db.setDatabaseName(ui->leDBName->text());       //这里输入你的数据库名
    m_db.setUserName(ui->leUser->text());
    m_db.setPassword(ui->lePwd->text());   //这里输入你的密码
    if (!m_db.open()) {
        QMessageBox::critical(0, QObject::tr("无法打开数据库"),"无法创建数据库连接！ ", QMessageBox::Cancel);
        return;
    }
    //ui->lbStatus->setText("连接成功！");
    m_isDBOpened = true;
    ui->btnReset->setEnabled(true);
    ui->btnConfirm->setEnabled(false);
    ui->pbQuery->setEnabled(true);

    QSqlQuery query("select min(id) as minid ,max(id) as maxid from testdb",m_db);
    QSqlRecord rec = query.record();
    if(query.next())
    {
        int maxididx = rec.indexOf("maxid");
        int minididx = rec.indexOf("minid");
        QString maxid = query.value(maxididx).toString();
        QString minid = query.value(minididx).toString();
        ui->leStartLine->setText(minid);
        ui->leEndLine->setText(maxid);
        //qDebug()<<"sname:"<<value;
    }
    query.prepare("select min(timeflag) as mintime, max(timeflag) as maxtime from testdb");
    query.exec();
    if(query.next()){
        rec = query.record();
        int maxididx = rec.indexOf("maxtime");
        int minididx = rec.indexOf("mintime");
        QString maxtime = query.value(maxididx).toString();
        QString mintime = query.value(minididx).toString();
        ui->leStartTime->setText(mintime);
        ui->leEndTime->setText(maxtime);
    }

}

void DBInConfWidget::slotReset()
{
//    if(m_db.isOpen()){
//        m_db.close();
//    }
    m_isDBOpened = false;
    ui->btnConfirm->setEnabled(true);
    ui->btnReset->setEnabled(false);
    ui->pbQuery->setEnabled(false);
}

//void DBInConfWidget::slotInsertTeleData(QByteArray ba)
//{
//    Q_UNUSED(ba);
//    if(!m_isDBOpened){
//        ui->lbStatus->setText("数据库未连接！！！！");
//        return;
//    }
//    QDateTime recvDateTime = ParseDateTime(ba);

//    QSqlQuery query(m_db);
//    query.prepare("INSERT INTO testdb(modelno,bindata)" "VALUES(:modelno,:bindata)");
//    query.bindValue(":modelno","XT-1");
//    query.bindValue(":bindata",ba);
//    if(query.exec()){
//        m_savedItemCount++;
//        ui->lbStatus->setText(QString("保存 %1 条 %2").arg(m_savedItemCount).arg(recvDateTime.toString()));
//    }
//    else{
//        ui->lbStatus->setText("数据库存储失败");
//        qDebug() << query.lastError().text();
//    }
//}

bool DBInConfWidget::isDBConnected()
{
    return m_isDBOpened;
}

void DBInConfWidget::slotQuery()
{
    if(ui->rbLineLimit->isChecked()){
        QSqlQuery query(m_db);
        query.prepare("select bindata from testdb where id>= :minid and id <= :maxid ");
        query.bindValue(":minid",ui->leStartLine->text());
        query.bindValue(":maxid",ui->leEndLine->text());
        query.exec();
        while(query.next()){
            int dataid = query.record().indexOf("bindata");
            if(dataid < 0){
                qDebug() << query.lastError();
                break;

            }
            QByteArray ba = query.value(dataid).toByteArray();
            if(ba.size() >= (TC_SUBFRAMELEN + TC_SUBTIMELEN) * TC_SUBOFFULL){
                emit procFullFrame(ba);
            }
            else{
                qDebug() << "数据库中保存着无效的帧数据！！！！";
            }
        }
    }
    else{
        QSqlQuery query(m_db);
        query.prepare("select bindata from testdb where timeflag>= :mintime and timeflag <= :maxtime ");
        query.bindValue(":mintime",ui->leStartTime->text());
        query.bindValue(":maxtime",ui->leEndTime->text());
        query.exec();
        while(query.next()){
            QByteArray ba = query.value(0).toByteArray();
            if(ba.size() >= (TC_SUBFRAMELEN + TC_SUBTIMELEN) * TC_SUBOFFULL){
                emit procFullFrame(ba);
            }
            else{
                qDebug() << "数据库中保存着无效的帧数据！！！！";
            }
        }
        qDebug() << query.lastError();
    }
}
