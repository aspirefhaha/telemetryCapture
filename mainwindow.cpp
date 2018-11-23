#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DBInConfWidget.h"
#include "ZMQInWidget.h"
#include <qmessagebox.h>
#include <QDockWidget>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
	createDockWindows();
}

MainWindow::~MainWindow()
{
    //delete ui;
}


slots void MainWindow::hello1()
{
	QMessageBox::critical(NULL, "critical", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}



void MainWindow::createDockWindows()
{

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
	aboutAction->setIcon(QIcon("images/Qt.png"));

	QMenu *aboutMenu = menuBar->addMenu(tr("About"));
	aboutMenu->addAction(aboutAction);

	toolBar->addAction(aboutAction);

	QDockWidget * dock = new QDockWidget(tr("数据入"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	/*QListWidget * customerList = new QListWidget(dock);
	customerList->addItems(QStringList() << "John");*/
	/*Ui::DBConf dbconf;
	QWidget * customerWidget = new QWidget(this);
	dbconf.setupUi(customerWidget);*/
	DBInConfWidget * dbinconfDlg = new DBInConfWidget(this);

	//dock->setWidget(customerWidget);
	dock->setWidget(dbinconfDlg);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	dock = new QDockWidget(tr("串联入"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	ZMQInWidget * zmqin = new ZMQInWidget(this);

	//dock->setWidget(customerWidget);
	dock->setWidget(zmqin);
	addDockWidget(Qt::LeftDockWidgetArea, dock);


	dock = new QDockWidget(tr("Paragraphs"), this);
	QListWidget * paragraphList = new QListWidget(dock);
	paragraphList->addItems(QStringList() << "Thank");
	dock->setWidget(paragraphList);
	addDockWidget(Qt::RightDockWidgetArea, dock);

	connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));

	

}

void MainWindow::aboutQt()
{
	qApp->aboutQt();
}


