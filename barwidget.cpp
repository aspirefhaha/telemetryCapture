#include "barwidget.h"
#include <QBarCategoryAxis>

#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QDebug>


barWidget::barWidget(QWidget *parent) : QWidget(parent),ui(new Ui::barwindow)
{
    ui->setupUi(this);
    //qDebug() << sizeof(m_pBarsets)/sizeof(QBarSet *);
    for(unsigned long i= 0 ;i< sizeof(m_pBarsets)/sizeof(QBarSet *);i++){
        //qDebug() << "in barset " <<i;
        m_pBarsets[i]= new QBarSet("John" + QString::number(i));

        * m_pBarsets[i] << 10 << 10 << 10 <<  10 <<  10<<  10;
        //m_pSeries->append(m_pBarsets[i]);
    }

    m_pSeries = new QBarSeries();
    for(unsigned long i= 0 ;i< sizeof(m_pBarsets)/sizeof(QBarSet *);i++){
        //qDebug() << "in Series" <<  i;
        m_pSeries->append(m_pBarsets[i]);
    }
    m_pChart = new QChart();

    m_pChart->addSeries(m_pSeries);
    //qDebug() << "series size " << m_pChart->series().size();
    m_pChart->setTitle("Simple barchart example");
    m_pChart->setAnimationOptions(QChart::SeriesAnimations);

    m_pAxis = new QBarCategoryAxis();

    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    m_pAxis->append(categories);
    m_pChart->createDefaultAxes();//创建默认的左侧的坐标轴（根据 QBarSet 设置的值）

    m_pChart->legend()->setVisible(true); //设置图例为显示状态
    m_pChart->legend()->setAlignment(Qt::AlignBottom);//设置图例的显示位置在底部

    m_pChart->setAxisX(m_pAxis, m_pSeries);//设置坐标轴

    m_pChartview = new QChartView(m_pChart);
    m_pChartview->setRenderHint(QPainter::Antialiasing);

    ui->horizontalLayout->addWidget(m_pChartview);
}


void barWidget::updateBarData()
{
    //m_pChart->removeAllSeries();
    unsigned long selen = m_pChartview->chart()->series().size();
    //qDebug() << selen;
    for(unsigned long i= 0 ;i< selen;i++){
        QBarSeries * series = (QBarSeries* )m_pChartview->chart()->series().at(i);
        series->clear();
        for(unsigned long j = 0; j< sizeof(m_pBarsets)/sizeof(QBarSet *);j++){
            QBarSet * pbarset = new QBarSet("John" + QString::number(j));
            * pbarset<< qrand()%10 << qrand() % 10 << qrand() %10 << qrand() % 10 << qrand() % 10<< qrand() % 10 ;
            series->append(pbarset);
        }

    }


}
