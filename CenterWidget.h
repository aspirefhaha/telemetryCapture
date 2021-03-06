#pragma once

#include <QWidget>
#include "ui_centerwindow.h"
#include "barwidget.h"
#include "chartWidget.h"
#include "GLWidget.h"

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
    void postureChanged(qreal yaw,qreal pitch,qreal roll);
    void recvData(QByteArray);
    void clearDataList();

private:
    Ui::centerWidget *ui;
    chartWidget * chartwidget;
    barWidget * barwidget;
   SpaceTrekGLWidget * m_pGLWidget;
    void mockData();

public:
    void keyPressEvent( QKeyEvent* event );
    void wheelEvent( QWheelEvent* event );
    //void mouseMoveEvent(QMouseEvent * event);
};

