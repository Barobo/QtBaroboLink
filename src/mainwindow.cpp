#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "barobolink.h"
#include "controlpanel.h"
#include "connectdialog.h"
#include <QObject>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , comms_()
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    /* Set up the AsyncRobot object to track remote robots */
    asyncRobot_ = new AsyncRobot();
    /* Set up control panel */
    controlPanel_ = new ControlPanelForm(asyncRobot_, ui->tab_control);
    controlPanel_->setEnabled(false);
    controlPanel_->show();

    /* tab_control needs a QLayout for it to be able to manage its
     * controlPanel_ widget correctly (auto-expand, etc.). Couldn't set one
     * in QtCreator for some reason, so we have to set it here. Passing
     * tab_control to the QLayout constructor transfers ownership of the
     * QLayout object to tab_control (by calling tab_control->setLayout()). */
    QVBoxLayout* layout = new QVBoxLayout (ui->tab_control);
    layout->addWidget(controlPanel_);

    /* Set up auxiliary control panel */
    auxControlPanel_ = new AuxControlPanelForm(asyncRobot_, ui->tab_aux);
    auxControlPanel_->show();
    layout = new QVBoxLayout (ui->tab_aux);
    layout->addWidget(auxControlPanel_);

    /* Connect control panel signals */
    QObject::connect(controlPanel_, SIGNAL(setUIWidgetsState(bool)),
        auxControlPanel_, SLOT(setEnabled(bool)));

    /* Looks kinda stupid with a margin. */
    layout->setContentsMargins(0, 0, 0, 0);

    /* Set up connect panel */
    connectDialog_ = new ConnectDialogForm();
    /* addWidget parents connectDialog_ to layout_connectArea's parent. */
    ui->layout_connectArea->addWidget(connectDialog_);
    connectDialog_->show();

    /* Start comms forwarding */
    comms_ = new CommsForwarding;
    comms_->start();

    connectSignals();
}

int MainWindow::currentTab()
{
  return ui->tabWidget->currentIndex();
}

void MainWindow::errorDialog(const QString & msg)
{
  QMessageBox msgBox;
  msgBox.setText(msg);
  msgBox.exec();
}

void MainWindow::connectSignals ()
{
  QObject::connect(connectDialog_->tableView_Robots, SIGNAL(pressed(const QModelIndex &)),
      controlPanel_, SLOT(setActiveRobot(const QModelIndex&)));
  QObject::connect(connectDialog_, SIGNAL(activeRobotSelected(const QModelIndex &)),
      controlPanel_, SLOT(setActiveRobot(const QModelIndex&)));
  QObject::connect(robotManager(), SIGNAL(activeRobotSelected(int)), 
      controlPanel_, SLOT(setActiveRobot(int)));
}

