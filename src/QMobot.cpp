#include "QMobot.h"
#include "recordmobot.h"
#include <QDebug>
#include <QMessageBox>

QMobot::QMobot(QObject *parent)
  :QObject(parent)
{
}

QMobot::~QMobot()
{
}

int QMobot::connectWithAddress(const char address[], int channel)
{
  _connectStatus = RMOBOT_CONNECTING;
  emit connectStatusChanged(RMOBOT_CONNECTING);
  int rc = RecordMobot::connectWithAddress(address, channel);
  if(rc) {
    if(-2 == rc) {
      emit connectError(QString("ERROR: No Linkbot dongle detected. Please attach a Linkbot or "
            "Linkbot Dongle to the computer and try again."));
    } else if (-1 == rc) {
      emit connectError(QString("ERROR: Could not connect to robot. Please make sure the robot "
            "powered on and within range. The Linkbot range is typically 50-100 meters in "
            "typical operating conditions."));
    }
    emit connectStatusChanged(RMOBOT_NOT_CONNECTED);
  } else {
    emit connectStatusChanged(RMOBOT_CONNECTED);
  }
  return 0;
}

int QMobot::disconnectRobot()
{
    _connectStatus = RMOBOT_NOT_CONNECTED;
    emit connectStatusChanged(RMOBOT_NOT_CONNECTED);
    lock_.lock();
    int rc = CMobot::disconnect();
    lock_.unlock();
    return rc;
}
