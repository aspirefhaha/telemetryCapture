#include "commonutils.h"
#include <QTextCodec>
#include <QDateTime>

QString ToQString(const std::string& cstr)
{
    QTextCodec* pCodec = QTextCodec::codecForName("UTF-8");
    QString qstr;
    if(!pCodec)
        return qstr;

    qstr = pCodec->toUnicode(cstr.c_str(), cstr.length());
    return qstr;
}

QDateTime ParseDateTime(const QByteArray ba)
{
    QDateTime recvDateTime;
    unsigned int hour = ba.at(4) & 0x3f;
    hour = (hour >> 4) * 10 + (hour & 0xf);
    unsigned int min = ba.at(3);
    min = (min >> 4) * 10 + (min & 0xf);
    unsigned int sec = ba.at(2);
    // qDebug() << "second :" << sec << " hex " << QString::number(sec,16);
    sec = (sec >> 4) * 10 + (sec & 0xf);
    qreal msec ;
    int msec1 = ba.at(0);
    msec = ((msec1 >> 4 )&0xf) + (msec1 & 0xf) * 0.1;
    msec1 = ba.at(1);
    msec += ((msec1 >>4)&0xf) * 100 + (msec1 & 0xf) * 10;
    QTime recvtime(hour,min,sec,(int)msec);
    //qDebug() << " after qtime sec:" << recvtime.second() ;
    QDate recvdate = QDate::currentDate();
    int dayget = ba.at(5);
    int dayoy = (ba.at(4) >> 6) || ((dayget & 0x3 )<< 2);
    dayget = ba.at(6);
    dayoy += ((dayget >> 2) & 0xf ) * 10 + (dayget >> 6 ) * 100;
    int daydiff =  dayoy - recvdate.dayOfYear() ;
    recvdate.addDays(daydiff);
    recvDateTime.setDate(recvdate);
    // qDebug() << recvtime;
    recvDateTime.setTime(recvtime);
    // qDebug() << recvDateTime;
    //ui->listWidget->insertItem(0,recvDateTime.toString());
    return recvDateTime;
}
