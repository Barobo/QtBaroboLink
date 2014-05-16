#include "qtrobotmanager.h"
#include "tcpcomms.h"

void eventCallback(const uint8_t* buf, int size, void* userdata)
{
  CommsRobotClient *client = static_cast<CommsRobotClient*>(userdata);
  QByteArray bytes((const char*)buf, size);
  client->sendDataToClient(bytes);
}

CommsRobotClient::CommsRobotClient(QObject *parent) : QObject(parent)
{
}

CommsRobotClient::~CommsRobotClient()
{
}

void CommsRobotClient::init(QTcpSocket *socket, RecordMobot* robot)
{
  sock_ = socket;
  robot_ = robot;
  QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(bytesFromClientReady()));
  QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
}

void CommsRobotClient::bytesFromClientReady()
{
  int rc;
  QByteArray tmpbuf;
  quint8 bytebuf[128];
  tmpbuf = sock_->readAll();
  recvbuf_ += tmpbuf;
  if( (recvbuf_.size() > 2) &&
      (recvbuf_.size() >= recvbuf_.at(1)) )
  {
    /* Received whole message */
    memcpy(bytebuf, recvbuf_.data(), recvbuf_.at(1));
    rc = robot_->transactMessage(bytebuf[0], &bytebuf[2], bytebuf[1]-3);
    rc = sock_->write((const char*)&bytebuf[2], bytebuf[3]);
    tmpbuf = recvbuf_.right(recvbuf_.size() - recvbuf_.at(1));
    recvbuf_ = tmpbuf;
  }
}

void CommsRobotClient::disconnect()
{
  robot_->setBound(false);
  recvbuf_.clear();
}

void CommsRobotClient::sendDataToClient(const QByteArray &bytearray)
{
  sock_->write(bytearray);
}

CommsForwarding::CommsForwarding(QObject *parent)
    : QObject(parent)
    , server_() { }

CommsForwarding::~CommsForwarding() { }

void CommsForwarding::start(quint16 port)
{
  if(server_.isListening()) return;
  server_.listen(QHostAddress::Any, port);
  QObject::connect(&server_, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void CommsForwarding::stop()
{
  server_.close();
}

void CommsForwarding::newConnection()
{
  qDebug() << "Received new connection.";
  /* Listener received a new connection. */
  /* See if we can get an unbound robot */
  QTcpSocket* sock;
  sock = server_.nextPendingConnection();
  RecordMobot* robot = robotManager()->getUnboundMobot();
  if(NULL == robot) {
    /* Immediately close the connection and return */
    sock->close();
    return;
  }
  /* Create new CommsRobotClient object */
  robot->setBound(true);
  CommsRobotClient* client = new CommsRobotClient();
  client->init(sock, robot);
  clients_.prepend(client);
  robot->enableEventCallback(eventCallback, client);
  qDebug() << "Finished receiving new connection.";
  emit newClientConnected();
}

