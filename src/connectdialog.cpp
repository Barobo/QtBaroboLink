#include <QAbstractItemView>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QString>
#include <mobot.h>
#include "barobolink.h"
#include "connectdialog.h"
#include "qtrobotmanager.h"

class ConnectDialogForm * g_ConnectDialogForm = 0;

ConnectDialogForm::ConnectDialogForm(QWidget *parent)
  : QWidget(parent)
{
  setupUi(this);
  QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy1.setHorizontalStretch(2);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  this->setSizePolicy(sizePolicy1);

  tableView_Robots->setModel(robotManager());
  tableView_Robots->setColumnWidth(0, 30);
  tableView_Robots->setColumnWidth(1, 170);
  tableView_Robots->setSelectionBehavior(QAbstractItemView::SelectRows);

  scanDialog_ = new ScanDialog();
  scanList_ = new ScanList(0);
  qRegisterMetaType< QList<QPersistentModelIndex> > ("QList<QPersistentModelIndex>");
  qRegisterMetaType< QAbstractItemModel::LayoutChangeHint > ("QAbstractItemModel::LayoutChangeHint");
  scanDialog_->scannedListView->setModel(scanList_);

  g_ConnectDialogForm = this;

  connectSignals();
}

ConnectDialogForm::~ConnectDialogForm()
{
}

void ConnectDialogForm::scanCallbackWrapper(const char* serialID)
{
  if(NULL != g_ConnectDialogForm) {
    g_ConnectDialogForm->scanCallback(serialID);
  }
}

void ConnectDialogForm::scanCallback(const char* serialID)
{
  /* Add the new entry into the scanlist */
  scanList_->newRobot(QString(serialID));
}

void ConnectDialogForm::selectRow(const QModelIndex &index)
{
  tableView_Robots->selectRow(index.row());
}

void ConnectDialogForm::addRobotFromLineEdit()
{
  QString robotID;
  robotID = edit_robotID->text();
  qDebug() << "Add robot " << robotID;
  robotManager()->addEntry(robotID);
}

void ConnectDialogForm::scanRobots()
{
  static mobot_t* dongle = NULL;
  if(NULL == dongle) {
    dongle = Mobot_getDongle();
    if(NULL == dongle) {
      Mobot_initDongle();
      dongle = Mobot_getDongle();
      if(NULL == dongle) {
        QMessageBox msgBox;
        msgBox.setText(
            "ERROR: No Linkbot dongle detected. Please attach a Linkbot or "
            "Linkbot Dongle to the computer and try again.");
        msgBox.exec();
        return;
      }
    }
  }
  scanList_->clearAll();
  /* Add the dongle's ID to the list */
  Mobot_getID(dongle);
  qDebug() << dongle->serialID;
  scanList_->newRobot(QString(dongle->serialID));
  scanDialog_->show();
  Mobot_registerScanCallback(dongle, scanCallbackWrapper);
  Mobot_queryAddresses(dongle);
}

void ConnectDialogForm::displayContextMenu(const QPoint &/*p*/)
{
  QMenu menu;
  QAction *connectaction = menu.addAction("Connect");
  QAction *disconnectaction = menu.addAction("Disconnect");
  QAction *removeaction = menu.addAction("Remove");
  if(robotManager()->isConnected(robotManager()->activeIndex())) {
    connectaction->setEnabled(false);
  } else {
    disconnectaction->setEnabled(false);
  }
  QObject::connect(connectaction, SIGNAL(triggered()), this, SLOT(connectIndices()));
  QObject::connect(disconnectaction, SIGNAL(triggered()), this, SLOT(disconnectIndices()));
  QObject::connect(removeaction, SIGNAL(triggered()), this, SLOT(removeIndices()));
  menu.exec(QCursor::pos());
}

void ConnectDialogForm::connectIndices()
{
  QModelIndexList selected = tableView_Robots->selectionModel()->selectedIndexes();
  for(int i = 0; i < selected.size(); i++) {
    QMetaObject::invokeMethod(
        robotManager(), "connectIndex", Qt::QueuedConnection, 
        Q_ARG(int, selected.at(i).row()));
  }
  if(selected.size() > 0) {
    emit activeRobotSelected(selected.at(0));
  }
}

void ConnectDialogForm::disconnectIndices()
{
  emit robotDisconnected();
  QModelIndexList selected = tableView_Robots->selectionModel()->selectedIndexes();
  for(int i = 0; i < selected.size()-1; i++) {
    robotManager()->disconnectIndex(selected.at(i).row());
  }
}

void ConnectDialogForm::moveUp()
{
  QModelIndexList selected = tableView_Robots->selectionModel()->selectedIndexes();
  if(selected.size() > 0) {
      robotManager()->moveEntryUp(selected.at(0).row());
  }
}

void ConnectDialogForm::moveDown()
{
  QModelIndexList selected = tableView_Robots->selectionModel()->selectedIndexes();
  if(selected.size() > 0) {
      robotManager()->moveEntryDown(selected.at(0).row());
  }
}

void ConnectDialogForm::removeIndices()
{
  QModelIndexList selected = tableView_Robots->selectionModel()->selectedRows();
  for(int i = selected.size()-1; i >= 0; i--) {
    robotManager()->remove(selected.at(i).row());
    robotManager()->write();
  }
}

void ConnectDialogForm::connectSignals(void)
{
  /* Set up robot tableView signals */
  tableView_Robots->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(tableView_Robots, SIGNAL(customContextMenuRequested(const QPoint&)),
      this, SLOT(displayContextMenu(const QPoint)));
  QObject::connect(tableView_Robots, SIGNAL(pressed(const QModelIndex &)),
      robotManager(), SLOT(setActiveIndex(const QModelIndex)));
  QObject::connect(tableView_Robots, SIGNAL(doubleClicked(const QModelIndex &)),
      robotManager(), SLOT(toggleConnection(const QModelIndex &)));

  /* Connect the signals for adding new robots */
  QObject::connect(edit_robotID, SIGNAL(returnPressed()),
      this, SLOT(addRobotFromLineEdit()));
  QObject::connect(button_addRobot, SIGNAL(clicked()),
      this, SLOT(addRobotFromLineEdit()));
  QObject::connect(button_scan, SIGNAL(clicked()),
      this, SLOT(scanRobots()));
  QObject::connect(scanDialog_->button_refresh, SIGNAL(clicked()),
      this, SLOT(scanRobots()));
  QObject::connect(this->pushButton_connect, SIGNAL(clicked()),
      this, SLOT(connectIndices()));
  QObject::connect(this->pushButton_disconnect, SIGNAL(clicked()),
      this, SLOT(disconnectIndices()));
  QObject::connect(this->pushButton_moveUp, SIGNAL(clicked()),
      this, SLOT(moveUp()));
  QObject::connect(this->pushButton_moveDown, SIGNAL(clicked()),
      this, SLOT(moveDown()));
  QObject::connect(this->pushButton_remove, SIGNAL(clicked()),
          this, SLOT(removeIndices()));
}
