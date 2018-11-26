#pragma once

#include <QWidget>
#include "ui_centerwindow.h"

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

private:
    Ui::centerWidget *ui;
    void mockData();
};
