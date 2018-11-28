#include "commonutils.h"
#include <QTextCodec>

QString ToQString(const std::string& cstr)
{
    QTextCodec* pCodec = QTextCodec::codecForName("UTF-8");
    QString qstr;
    if(!pCodec)
        return qstr;

    qstr = pCodec->toUnicode(cstr.c_str(), cstr.length());
    return qstr;
}
