#ifndef _QTROBOTMANAGER_H_
#define _QTROBOTMANAGER_H_

#include <QAbstractTableModel>
#include <QList>
#include <QPoint>
#include <QRunnable>
#include <QStyle>
#include <QThreadPool>
#include <QWidget>
#include "robotmanager.h"

/* Note: No need to instantiate this class explicitly. Use the robotManager()
 * access function. */
class QtRobotManager : public QAbstractTableModel, public CRobotManager
{
  Q_OBJECT
  public:
    QtRobotManager(QObject *parent);
    ~QtRobotManager();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    inline int activeIndex() const {return _activeIndex;}
    int read(const char* path);

  public slots:
    int connectIndex(int index);
    int disconnectIndex(int index);

    inline void setActiveIndex(int index) {_activeIndex = index;}
    void setActiveIndex(const QModelIndex &index);
    void clicked(const QModelIndex &index);
    void connectActiveIndex();
    void disconnectActiveIndex();
    void removeActiveIndex();
    void addEntry(QString entry);
    void toggleConnection(const QModelIndex &index);
    void refreshData();
    void displayMessageDialog(const QString & msg);

  signals:
    void activeRobotSelected(int index);


  private:
    int _activeIndex;
    bool disableEntry_[MAX_CONNECTED];
    QThreadPool *threadPool_;
};

QtRobotManager* robotManager();

#if 0
class RobotConnectWorker : public QObject, public QRunnable
{
  Q_OBJECT
  public:
    RobotConnectWorker(QObject *parent, int connectIndex);
    ~RobotConnectWorker();
    void run();

  signals:
    void connectStatusChanged();
    void connectFailed(const QString & msg);

  private:
    int connectIndex_;
};
#endif
#endif
