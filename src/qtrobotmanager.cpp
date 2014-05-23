#include "mainwindow.h"
#include "qtrobotmanager.h"
#include <QCursor>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QThreadPool>
#include <QVariant>

QtRobotManager::QtRobotManager(QObject *parent)
  :QAbstractTableModel(parent)
{
  threadPool_ = new QThreadPool(this);
  memset(disableEntry_, 0, sizeof(bool)*MAX_CONNECTED);
  int i;
  for(i = 0; i < numEntries(); i++) {
    QObject::connect(_mobots[i], SIGNAL(connectStatusChanged(int)), 
        this, SLOT(refreshData()));
  }

  qRegisterMetaType< QVector<int> > ("QVector<int>");
}

QtRobotManager::~QtRobotManager()
{
}

int QtRobotManager::connectIndex(int index)
{
  int rc = CRobotManager::connectIndex(index);
  if(rc == 0) {
      /* Check firmware version of robot */
      unsigned int version;
      QMobot* robot = getMobotIndex(index);
      robot->getVersions(version);
      if(version < Mobot_protocolVersion()) {
#ifdef __APPLE__
          emit errorMessage(QString("Warning: Robot ") + robot->getID() + 
                  QString(
                  " has old firmware. To update the firmware, "
                  "close BaroboLink and run the LinkbotFirmwareUpdate "
                  " utility located in your Applications folder."));
#else
          emit errorMessage(QString("Warning: Robot ") + robot->getID() + 
                  QString(
                  " has old firmware. To update the firmware, "
                  "close BaroboLink and run the LinkbotFirmwareUpdate "
                  " utility located in your start menu at "
                  "Start->BaroboLink->LinkbotFirmwareUpdate."));
#endif
      }
  }
  return rc;
}

int QtRobotManager::rowCount(const QModelIndex & ) const
{
  return numEntries();
}

int QtRobotManager::columnCount(const QModelIndex & ) const
{
  return 2;
}

QVariant QtRobotManager::data(const QModelIndex &index, int role) const
{
  if(role == Qt::DisplayRole) {
    if(index.column() == 0) {
      RecordMobot* mobot;
      mobot = robotManager()->getMobotIndex(index.row());
      if(mobot == NULL) {
        return QIcon(":/images/orb_red.png");
      }
      switch(mobot->connectStatus()) {
        case RMOBOT_NOT_CONNECTED:
          return QIcon(":/images/orb_red.png");
          break;
        case RMOBOT_CONNECTING:
          return QIcon(":/images/orb_gray.png");
          break;
        case RMOBOT_CONNECTED:
          return QIcon(":/images/orb_green.png");
          break;
      }
    } else if (index.column() == 1) {
      return QString(getEntry(index.row()));
    }
  }
  if(role == Qt::DecorationRole) {
    if(index.column() == 0) {
      RecordMobot* mobot;
      mobot = robotManager()->getMobotIndex(index.row());
      if(mobot == NULL) {
        return QIcon(":/images/orb_red.png");
      }
      switch(mobot->connectStatus()) {
        case RMOBOT_NOT_CONNECTED:
          return QIcon(":/images/orb_red.png");
          break;
        case RMOBOT_CONNECTING:
          return QIcon(":/images/orb_yellow.png");
          break;
        case RMOBOT_CONNECTED:
          return QIcon(":/images/orb_green.png");
          break;
        default:
          return QIcon(":/images/orb_red.png");
          break;
      }
    } 
  }
  if(role == Qt::ToolTipRole) {
    if(isConnected(index.row())) {
      return QString("Robot Connected.");
    } else {
      return QString("Robot Not Connected.");
    }
  }
  /*
  if(role == Qt::ForegroundRole) {
    if(index.column() == 1) {
      return QBrush(QColor("gray"));
    }
  }
  */
  return QVariant();
}

int QtRobotManager::read(const char* path)
{
  int rc = ConfigFile::read(path);
  if(rc) return rc;
  int i;
  for(i = 0; i < numEntries(); i++) {
    _mobots[i] = new QMobot(0);
    QObject::connect(_mobots[i], SIGNAL(connectStatusChanged(int)), 
        this, SLOT(refreshData()));
    QObject::connect(_mobots[i], SIGNAL(connectError(const QString &)), 
        mainWindow(), SLOT(errorDialog(const QString &)), Qt::QueuedConnection);
  }
  return 0;
}

void QtRobotManager::clicked(const QModelIndex &)
{
  qDebug() << "click.";
}

void QtRobotManager::setActiveIndex(const QModelIndex &index)
{
  _activeIndex = index.row();
  qDebug() << _activeIndex << " selected as active index in QtRobotManager.";
}

int QtRobotManager::disconnectIndex(int index)
{
  return CRobotManager::disconnect(index);
}

void QtRobotManager::connectActiveIndex()
{
  qDebug() << "Connect to index " << _activeIndex;
  int rc = QtRobotManager::connectIndex(_activeIndex);
  if(rc) {
    QMessageBox box;
    box.setText("Could not connect to robot. Please make sure there is a dongle plugged in and the robot is on.");
    box.exec();
  }
  emit dataChanged(createIndex(_activeIndex, 0), createIndex(_activeIndex, 1));
}

void QtRobotManager::disconnectActiveIndex()
{
  CRobotManager::disconnect(_activeIndex);
}

int QtRobotManager::moveEntryUp(const QModelIndex &index)
{
    return moveEntryUp(index.row());
}

int QtRobotManager::moveEntryUp(int index)
{
    int rc = CRobotManager::moveEntryUp(index);
    emit layoutChanged();
    return rc;
}

int QtRobotManager::moveEntryDown(const QModelIndex &index)
{
    return moveEntryDown(index.row());
}

int QtRobotManager::moveEntryDown(int index)
{
    int rc = CRobotManager::moveEntryDown(index);
    emit layoutChanged();
    return rc;
}

void QtRobotManager::removeActiveIndex()
{
  disconnectActiveIndex();
  ConfigFile::remove(_activeIndex);
  ConfigFile::write();
  emit layoutChanged();
}

void QtRobotManager::addEntry(QString entry)
{
  CRobotManager::addEntry(entry.toStdString());
  emit layoutChanged();
}

void QtRobotManager::toggleConnection(const QModelIndex &index)
{
  if(isConnected(index.row())) {
    QtRobotManager::disconnectIndex(index.row());
  } else {
    QtRobotManager::connectIndex(index.row());
  }
  emit layoutChanged();
}

void QtRobotManager::refreshData()
{
  emit dataChanged(createIndex(0, 0, (void*)NULL), createIndex(numEntries(), 0, (void*)NULL));
  emit layoutChanged();
  emit activeRobotSelected(_activeIndex);
}

void QtRobotManager::displayMessageDialog(const QString & msg)
{
  QMessageBox *b = new QMessageBox(0);
  b->setText(msg);
  b->exec();
  delete b;
}

QtRobotManager* robotManager()
{
  static QThread *robotManagerThread = new QThread(0);
  static QtRobotManager *instance = 0;
  if(0 == instance) {
    instance = new QtRobotManager(0);
    instance->read( Mobot_getConfigFilePath() );
    instance->moveToThread(robotManagerThread);
    robotManagerThread->start();
  }
  return instance;
}

