#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <string>

#include "DBOutWidget.h"
#include "CenterWidget.h"
#include "TelemetryInWidget.h"
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QToolBar * toolBar;
    QStatusBar * statusBar;
    QLabel * statusLabel;

public slots:
    void status(QString);
    void procFullFrame(QByteArray ba);
	void aboutQt();
signals:
    void postureChanged(qreal,qreal,qreal);
    void recvData(QByteArray);
//    void statusSignal(QString);
public:
    void createDockWindows();
    static QString ToQString(const std::string& cstr);

private :
    TelemetryInWidget * m_telein;
    CenterWidget * m_centerWidget;
    DBOutWidget * m_dbout;
};

#endif // MAINWINDOW_H
