#ifndef CONTROLPANEL_H__
#define CONTROLPANEL_H__

#include <QModelIndex>
#include "ui_controlpanel.h"
#include "asyncrobot.h"

class ControlPanelForm : public QWidget, private Ui::ControlPanelForm
{
  Q_OBJECT
  public:
    ControlPanelForm(AsyncRobot *asyncRobot, QWidget *parent = 0);

  public slots:
    void setActiveRobot(int index);
    void setActiveRobot(const QModelIndex &index);
    void driveJoint1To(int);
    void driveJoint2To(int);
    void enable(int state);
    void setJ1Label(int value);
    void setJ2Label(int value);
    void setJ1Label(double value);
    void setJ2Label(double value);
    void setJ1Label(const QString &value);
    void setJ2Label(const QString &value);

    void setJ2LabelText(const QString &value);

    void j1forward_handler();
    void j1backward_handler();
    void j1stop_handler();
    void j2forward_handler();
    void j2backward_handler();
    void j2stop_handler();

    void setSpeed1Label(double value);
    void setSpeed1Label(int value);
    void speed1EntryActivated();
    void setSpeed2Label(double value);
    void setSpeed2Label(int value);
    void speed2EntryActivated();

    void moveMotorButtonClicked();

signals:
    void beginMovingJoint(int joint, int direction);
    void stopJoint(int joint);
    void speed1Changed(int);
    void speed2Changed(int);
    void setUIWidgetsState(bool state);

  private:
    void startControlThread();
    int enabled_;
    AsyncRobot *asyncrobot_;
    QThread *mobotthread_;
};

#endif

