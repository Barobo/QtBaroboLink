#ifndef CONNECTDIALOG_H_
#define CONNECTDIALOG_H_

#include "ui_connectpanel.h"
#include "scandialog.h"
#include "scanlist.h"

class ConnectDialogForm : public QWidget, public Ui::ConnectDialogForm
{
  Q_OBJECT
  public:
    explicit ConnectDialogForm(QWidget* parent = 0);
    ~ConnectDialogForm();
    static void scanCallbackWrapper(const char* serialID);
    void scanCallback(const char* serialID);

  public slots:
    void selectRow(const QModelIndex &index);
    void addRobotFromLineEdit();
    void scanRobots();
    void displayContextMenu(const QPoint &p);
    void connectIndices();
    void disconnectIndices();
    void moveUp();
    void moveDown();
    void removeIndices();

  signals:
    void activeRobotSelected(const QModelIndex &index);
    void robotDisconnected();

  private:
    void connectSignals(void);
    ScanDialog *scanDialog_;
    ScanList *scanList_;
};

extern class ConnectDialogForm * g_ConnectDialogForm;
#endif

