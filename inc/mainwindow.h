#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "auxcontrolpanel.h"
#include "controlpanel.h"
#include "connectdialog.h"
#include "tcpcomms.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();
    int currentTab();

public slots:
    void errorDialog(const QString & msg);

private:
    Ui::MainWindow *ui;
    AsyncRobot *asyncRobot_;
    ControlPanelForm *controlPanel_;
    AuxControlPanelForm *auxControlPanel_;
    ConnectDialogForm *connectDialog_;
    CommsForwarding *comms_;
    void connectSignals();
};

MainWindow* mainWindow();
#endif // MAINWINDOW_H
