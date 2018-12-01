#pragma once

#include <QString>
#include <string>
#include <QByteArray>
#include <QDateTime>

QString ToQString(const std::string& cstr);
QDateTime ParseDateTime(const QByteArray ba);
