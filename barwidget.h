#pragma once

#include <QWidget>
#include <QBarSeries>
#include <QtCharts/QChartView>

#include <QtCharts/QBarCategoryAxis>
#include "ui_barwindow.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
    class barWidget;
}

class barWidget : public QWidget
{
    Q_OBJECT
public:
    explicit barWidget(QWidget *parent = 0);

signals:

public slots:
    void updateBarData();

private:
    Ui::barwindow * ui;
    QBarSet * m_pBarsets[6];
    QBarSeries * m_pSeries ;
    QChartView * m_pChartview;
    QChart * m_pChart;
    QBarCategoryAxis *m_pAxis;
};

