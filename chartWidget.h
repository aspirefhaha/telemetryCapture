#pragma once

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QVector>
#include <QTimer>

#include "chartView.h"
#include "callout.h"
#include "ui_chartwindow.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class chartWidget;
}

class chartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit chartWidget(QWidget *parent = 0);
    ~chartWidget();

private:
    void wheelEvent(QWheelEvent *event);

    void initUI();
    void initChart();
    void initSlot();

    void updateData();

    Ui::chartwindow *ui;

    ChartView *chartView;
    QChart *chart;
    Callout *tip;
    QLineSeries *series;

    QTimer *timer;

    quint16 count;

    bool isStopping;

private slots:
    void timerSlot();
    void buttonSlot();
    void tipSlot(QPointF position, bool isHovering);

};

