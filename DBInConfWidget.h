#pragma once
#include <QWidget>
#include <QSqlDatabase>
#include <QDateTime>
#include "ui_dbinconf.h"

namespace Ui {
	class DBInConfWidget;
}

class DBInConfWidget :
	public QWidget
{
	Q_OBJECT
public:
    DBInConfWidget(QWidget * parent=NULL);
	~DBInConfWidget();


private:
	Ui::DBConf *ui;
    QSqlDatabase m_db;

    bool m_isDBOpened;

public:
    QSize sizeHint() const;
    bool isDBConnected();

signals:
    void procFullFrame(QByteArray ba);

public slots:
    void ckAllControls();
    void slotConfirm();
    void slotReset();
    void slotQuery();
    //void slotInsertTeleData(QByteArray);
};

