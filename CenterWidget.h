#pragma once

#include <QWidget>
#include "ui_centerwindow.h"
#include "barwidget.h"

namespace Ui {
    class CenterWidget;
}

class CenterWidget:
        public QWidget
{
    Q_OBJECT
public:
    CenterWidget(QWidget * parent=NULL);
    ~CenterWidget();

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_centerTabChanged(int idx);

private:
    Ui::centerWidget *ui;
    QWidget * chartwidget;
    barWidget * barwidget;
    void mockData();
};

