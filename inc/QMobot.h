#ifndef __QMOBOT_H__
#define __QMOBOT_H__

#include "recordmobot.h"
#include <QObject>
#include <QMutex>

class QMobot : public QObject, public RecordMobot
{
  Q_OBJECT
  public:
    QMobot(QObject *parent);
    ~QMobot();

    int connectWithAddress(const char address[], int channel);
    int disconnectRobot();
    void lock() {lock_.lock();}
    void unlock() {lock_.unlock();}

  signals:
    void connectStatusChanged(int connectStatus);
    void connectError(const QString & msg);

  private:
    QMutex lock_;
};

#endif
