#include "recordmobot.h"
#include <QObject>

class QMobot : public QObject, public RecordMobot
{
  Q_OBJECT
  public:
    QMobot(QObject *parent);
    ~QMobot();

    int connectWithAddress(const char address[], int channel);
    int disconnectRobot();

  signals:
    void connectStatusChanged(int connectStatus);
    void connectError(const QString & msg);
};
