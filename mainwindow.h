#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <string>

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
	void aboutQt();
//signals:
//    void statusSignal(QString);
public:
    void createDockWindows();
    static QString ToQString(const std::string& cstr);
};

#endif // MAINWINDOW_H
