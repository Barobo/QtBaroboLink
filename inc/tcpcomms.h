#ifndef TCPCOMMS_H_
#define TCPCOMMS_H_

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "recordmobot.h"

class CommsRobotClient : public QObject
{
  Q_OBJECT
  public:
    CommsRobotClient(QObject *parent = 0);
    ~CommsRobotClient();
    void init(QTcpSocket *socket, RecordMobot* robot);

  public slots:
    void bytesFromClientReady();
    void disconnect();
    void sendDataToClient(const QByteArray &bytearray);

  private:
    QTcpSocket *sock_;
    RecordMobot* robot_;
    QByteArray recvbuf_;
};

class CommsForwarding : public QObject
{
  Q_OBJECT
  public:
    CommsForwarding(QObject *parent = 0);
    ~CommsForwarding();
    void start(quint16 port = 5768);
    void stop();

  public slots:
    void newConnection();

  private:
    QTcpServer server_;
    QList<CommsRobotClient*> clients_;
};

#endif
