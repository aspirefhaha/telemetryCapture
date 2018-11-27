#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DBInConfWidget.h"
#include "ZMQInWidget.h"
#include "ZMQOutWidget.h"
#include "DBOutWidget.h"
#include "CenterWidget.h"
#include "TelemetryInWidget.h"
#include <qmessagebox.h>
#include <QDockWidget>
#include <QListWidget>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
	QMenuBar * menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	QToolBar* toolBar = new QToolBar(this);
	addToolBar(toolBar);

	QWidget * centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	QStatusBar * statusBar = new QStatusBar(this);
	setStatusBar(statusBar);


	QAction *aboutAction = new QAction(tr("AboutQt"), this);
	aboutAction->setStatusTip(tr("About Qt"));
    aboutAction->setIcon(QIcon(":/images/qt.png"));

	QMenu *aboutMenu = menuBar->addMenu(tr("About"));
	aboutMenu->addAction(aboutAction);

	toolBar->addAction(aboutAction);
	createDockWindows();

	connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));
    setMinimumSize(1200, 580);
}

MainWindow::~MainWindow()
{
    //delete ui;
}


slots void MainWindow::hello1()
{
	QMessageBox::critical(NULL, "critical", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

QString MainWindow::ToQString(const string& cstr)
{
    QTextCodec* pCodec = QTextCodec::codecForName("gb2312");
    if(!pCodec) return "";

    QString qstr = pCodec->toUnicode(cstr.c_str(), cstr.length());
    return qstr;
}


void MainWindow::createDockWindows()
{
    QDockWidget * dock = new QDockWidget(ToQString("ң����"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	TelemetryInWidget * telein = new TelemetryInWidget(this);
	dock->setWidget(telein);
	addDockWidget(Qt::LeftDockWidgetArea, dock); 
	
    dock = new QDockWidget(ToQString("���ݿ���"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	DBInConfWidget * dbinconfDlg = new DBInConfWidget(this);
	dock->setWidget(dbinconfDlg);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

    dock = new QDockWidget(ToQString("������"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	ZMQInWidget * zmqin = new ZMQInWidget(this);
	dock->setWidget(zmqin);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

    dock = new QDockWidget(ToQString("������"), this);
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	ZMQOutWidget * zmqout = new ZMQOutWidget(this);
	dock->setWidget(zmqout);
	addDockWidget(Qt::RightDockWidgetArea, dock);

    dock = new QDockWidget(ToQString("���ݿ��"), this);
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	DBOutWidget * dbout = new DBOutWidget(this);
	dock->setWidget(dbout);
	addDockWidget(Qt::RightDockWidgetArea, dock);

    CenterWidget * centerWidget = new CenterWidget(this);
    setCentralWidget(centerWidget);

	/*dock = new QDockWidget(tr("Paragraphs"), this);
	QListWidget * paragraphList = new QListWidget(dock);
	paragraphList->addItems(QStringList() << "Thank");
	dock->setWidget(paragraphList);
	addDockWidget(Qt::RightDockWidgetArea, dock);*/

}

void MainWindow::aboutQt()
{
	qApp->aboutQt();
}


