#include "chartWidget.h"
#include "ui_chartwindow.h"

#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtMath>


chartWidget::chartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chartwindow),
    chart(new QChart),
    tip(0),
    timer(new QTimer),
    count(0),
    isStopping(false)
{
    ui->setupUi(this);

    initUI();

    timer->setInterval(100);
    timer->start();

    initSlot();
}

chartWidget::~chartWidget()
{
    delete ui;
}

void chartWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        chart->zoom(1.1);
    } else {
        chart->zoom(10.0/11);
    }

    QWidget::wheelEvent(event);
}

void chartWidget::initUI()
{

    initChart();
}

#define XRANGE  960

void chartWidget::initChart()
{
    for(int i = 0 ;i < LINESENUM;i++){
           m_pSeries[i] = new QLineSeries(this);
           chart->addSeries(m_pSeries[i]);
    }

    //series->setUseOpenGL(true);//openGl 加速
    //qDebug()<<series->useOpenGL();


//    series->setUseOpenGL(true);

    chart->createDefaultAxes();
    chart->axisY()->setRange(-10, 380);
    chart->axisX()->setRange(0, 960);

    chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisX()->setTitleText("Time/ 0.1 sec");
    chart->axisY()->setTitleText("Degree");

    chart->axisX()->setGridLineVisible(false);
    chart->axisY()->setGridLineVisible(false);

    /* hide legend */
    chart->legend()->hide();

    chartView = new ChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->horizontalLayout->addWidget(chartView);
}

void chartWidget::initSlot()
{
    connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    //connect(ui->stopBtn, SIGNAL(clicked(bool)), this, SLOT(buttonSlot()));
    for(int i =0 ;i<LINESENUM; i++)
        connect(m_pSeries[i], SIGNAL(hovered(QPointF, bool)), this, SLOT(tipSlot(QPointF,bool)));
}

void chartWidget::updateRawData(qreal *datas)
{
    for(int i = 0;i<LINESENUM;i++){
        rawdata[i] = datas[i];
    }
}

void chartWidget::updateData()
{
    for(int j = 0;j<LINESENUM;j++){
        int i;

        QVector<QPointF> oldData = m_pSeries[j]->pointsVector();
        QVector<QPointF> data;

        if (oldData.size() < 961) {
            data = m_pSeries[j]->pointsVector();
        } else {
            /* 添加之前老的数据到新的vector中，不复制最前的数据，即每次替换前面的数据
             * 由于这里每次只添加1个数据，所以为1，使用时根据实际情况修改
             */
            for (i = 1; i < oldData.size(); ++i) {
                data.append(QPointF(i - 1 , oldData.at(i).y()));
            }
        }

        qint64 size = data.size();
        /* 这里表示插入新的数据，因为每次只插入1个，这里为i < 1,
         * 但为了后面方便插入多个数据，先这样写
         */
        for(i = 0; i < 1; ++i){
            data.append(QPointF(i + size,rawdata[j]));
        }

        m_pSeries[j]->replace(data);

    }

    count++;
}

void chartWidget::timerSlot()
{
    if (QObject::sender() == timer) {
        updateData();
    }
}

void chartWidget::buttonSlot()
{
//    if (QObject::sender() == ui->stopBtn) {
//        if (!isStopping) {
//            timer->stop();
//            ui->stopBtn->setText("Start");
//        } else {
//            ui->stopBtn->setText("Stop");
//            timer->start();
//        }
//        isStopping = !isStopping;
//    }
}

void chartWidget::tipSlot(QPointF position, bool isHovering)
{
    if (tip == 0)
        tip = new Callout(chart);

    if (isHovering) {
        tip->setText(QString("X: %1 \nY: %2 ").arg(position.x()).arg(position.y()));
        tip->setAnchor(position);
        tip->setZValue(11);
        tip->updateGeometry();
        tip->show();
    } else {
        tip->hide();
    }
}

