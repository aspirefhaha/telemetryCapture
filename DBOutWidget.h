#pragma once
#include <QWidget>
#include <QSqlDatabase>
#include <QDateTime>
#include "ui_dbout.h"

namespace Ui {
	class DBOutWidget;
}
class DBOutWidget :
	public QWidget
{
    Q_OBJECT

public:
    explicit DBOutWidget(QWidget * parent);
    ~DBOutWidget();

private :
	Ui::DBOut * ui;
    QSqlDatabase m_db;

    bool m_isDBOpened;
    int m_savedItemCount;

public:
    QSize sizeHint() const;
    bool isDBConnected();

public slots:
    void slotConfirm();
    void slotReset();
    void slotInsertTeleData(QByteArray);
};

