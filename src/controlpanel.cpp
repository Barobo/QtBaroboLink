#include "controlpanel.h"
#include <QDebug>
#include <QThread>
#include <QModelIndex>
#include <math.h>
#include "asyncrobot.h"
#include "recordmobot.h"
#include "qtrobotmanager.h"

ControlPanelForm::ControlPanelForm(AsyncRobot* asyncRobot, QWidget *parent)
  : QWidget(parent)
{
  setupUi(this);
  asyncrobot_ = asyncRobot;
  mobotthread_ = new QThread();
  asyncrobot_->moveToThread(mobotthread_);
  mobotthread_->start();

  /* Connect motor drive buttons */
  QObject::connect(this->button_j1Forward, SIGNAL(clicked()),
      this, SLOT(j1forward_handler()));
  QObject::connect(this->button_j1Stop, SIGNAL(clicked()),
      this, SLOT(j1stop_handler()));
  QObject::connect(this->button_j1Backward, SIGNAL(clicked()),
      this, SLOT(j1backward_handler()));
  QObject::connect(this->button_j2Forward, SIGNAL(clicked()),
      this, SLOT(j2forward_handler()));
  QObject::connect(this->button_j2Stop, SIGNAL(clicked()),
      this, SLOT(j2stop_handler()));
  QObject::connect(this->button_j2Backward, SIGNAL(clicked()),
      this, SLOT(j2backward_handler()));

  /* Connect motor position dials */
  QObject::connect(this->dial_j1, SIGNAL(valueChanged(int)),
      this, SLOT(driveJoint1To(int)));    
  QObject::connect(this->dial_j1, SIGNAL(sliderPressed()),
      asyncrobot_, SLOT(disableJointSignals()));    
  QObject::connect(this->dial_j1, SIGNAL(sliderPressed()),
      asyncrobot_, SLOT(acquireJointControl()));    
  QObject::connect(this->dial_j1, SIGNAL(sliderReleased()),
      asyncrobot_, SLOT(enableJointSignals()));    
  QObject::connect(this->dial_j1, SIGNAL(sliderReleased()),
      asyncrobot_, SLOT(releaseJointControl()));    

  QObject::connect(this->dial_j2, SIGNAL(valueChanged(int)),
      this, SLOT(driveJoint2To(int)));    
  QObject::connect(this->dial_j2, SIGNAL(sliderPressed()),
      asyncrobot_, SLOT(disableJointSignals()));    
  QObject::connect(this->dial_j2, SIGNAL(sliderPressed()),
      asyncrobot_, SLOT(acquireJointControl()));    
  QObject::connect(this->dial_j2, SIGNAL(sliderReleased()),
      asyncrobot_, SLOT(enableJointSignals()));    
  QObject::connect(this->dial_j2, SIGNAL(sliderReleased()),
      asyncrobot_, SLOT(releaseJointControl()));    

  QObject::connect(asyncrobot_, SIGNAL(joint1Changed(int)),
      this->dial_j1, SLOT(setValue(int)));
  QObject::connect(asyncrobot_, SIGNAL(joint1Changed(double)),
      this, SLOT(setJ1Label(double)));
  QObject::connect(asyncrobot_, SIGNAL(joint2Changed(int)),
      this->dial_j2, SLOT(setValue(int)));
  QObject::connect(asyncrobot_, SIGNAL(joint2Changed(double)),
      this, SLOT(setJ2Label(double)));

  /* Connect "Enable" checkbox */
  QObject::connect(this->checkBox_enable, SIGNAL(stateChanged(int)), 
      this, SLOT(enable(int)));
  QObject::connect(this->checkBox_enable, SIGNAL(stateChanged(int)),
      asyncrobot_, SLOT(setState(int)));

  QObject::connect(this, SIGNAL(beginMovingJoint(int, int)),
      asyncrobot_, SLOT(moveJoint(int, int)));
  QObject::connect(this, SIGNAL(stopJoint(int)),
      asyncrobot_, SLOT(stopJoint(int)));

  /* Connect drive buttons */
  QObject::connect(this->button_driveForward, SIGNAL(clicked()),
      asyncrobot_, SLOT(moveForward()));
  QObject::connect(this->button_driveBackward, SIGNAL(clicked()),
      asyncrobot_, SLOT(moveBackward()));
  QObject::connect(this->button_driveLeft, SIGNAL(clicked()),
      asyncrobot_, SLOT(turnLeft()));
  QObject::connect(this->button_driveRight, SIGNAL(clicked()),
      asyncrobot_, SLOT(turnRight()));

  QObject::connect(this->button_stop, SIGNAL(clicked()),
      asyncrobot_, SLOT(stop()));
  QObject::connect(this->button_moveToZero, SIGNAL(clicked()),
      asyncrobot_, SLOT(resetToZero()));

  /* Connect speed sliders */
  this->slider_speed1->setRange(0, 200);
  this->slider_speed2->setRange(0, 200);
  QObject::connect(asyncrobot_, SIGNAL(speed1Changed(int)),
      this->slider_speed1, SLOT(setValue(int)));
  QObject::connect(asyncrobot_, SIGNAL(speed1Changed(double)),
      this, SLOT(setSpeed1Label(double)));
  QObject::connect(this->slider_speed1, SIGNAL(valueChanged(int)),
      asyncrobot_, SLOT(setSpeed1(int)));
  QObject::connect(this->slider_speed1, SIGNAL(valueChanged(int)),
      this, SLOT(setSpeed1Label(int)));
  QObject::connect(this->edit_speed1, SIGNAL(returnPressed()),
      this, SLOT(speed1EntryActivated()));

  QObject::connect(asyncrobot_, SIGNAL(speed2Changed(int)),
      this->slider_speed2, SLOT(setValue(int)));
  QObject::connect(asyncrobot_, SIGNAL(speed2Changed(double)),
      this, SLOT(setSpeed2Label(double)));
  QObject::connect(this->slider_speed2, SIGNAL(valueChanged(int)),
      asyncrobot_, SLOT(setSpeed2(int)));
  QObject::connect(this->slider_speed2, SIGNAL(valueChanged(int)),
      this, SLOT(setSpeed2Label(int)));
  QObject::connect(this->edit_speed2, SIGNAL(returnPressed()),
      this, SLOT(speed2EntryActivated()));

}
 
void ControlPanelForm::driveJoint1To(int angle)
{
  /* Negative angles because the directionality of the Qt dials is opposite of
   * our joint directions. */
  asyncrobot_->driveJointTo(1, -angle);
  setJ1Label(-angle);
}

void ControlPanelForm::driveJoint2To(int angle)
{
  asyncrobot_->driveJointTo(2, -angle);
  asyncrobot_->driveJointTo(3, -angle);
  setJ2Label(-angle);
}

void ControlPanelForm::enable(int state)
{
  enabled_ = state;
  bool enable;
  if(state) {
    enable = true;
  } else {
    enable = false;
  }
  this->groupBox->setEnabled(enable);
  this->groupBox_2->setEnabled(enable);
  this->groupBox_3->setEnabled(enable);
  this->groupBox_4->setEnabled(enable);
}

void ControlPanelForm::setJ1Label(int value)
{
  setJ1Label(QString("%1").arg(value));
}

void ControlPanelForm::setJ2Label(int value)
{
  setJ2Label(QString("%1").arg(value));
}

void ControlPanelForm::setJ1Label(double value)
{
  setJ1Label(QString("%1").arg(value, 0, 'f', 1));
}

void ControlPanelForm::setJ2Label(double value)
{
  setJ2Label(QString("%1").arg(value, 0, 'f', 1));
}

void ControlPanelForm::setJ1Label(const QString &value)
{
  this->label_j1_angle->setText(value);  
}

void ControlPanelForm::setJ2Label(const QString &value)
{
  this->label_j2_angle->setText(value);  
}

void ControlPanelForm::j1forward_handler()
{
  emit beginMovingJoint(1, 1);
}

void ControlPanelForm::j1backward_handler()
{
  emit beginMovingJoint(1, 2);
}

void ControlPanelForm::j1stop_handler()
{
  emit stopJoint(1);
}

void ControlPanelForm::j2forward_handler()
{
  emit beginMovingJoint(2, 1);
}

void ControlPanelForm::j2backward_handler()
{
  emit beginMovingJoint(2, 2);
}

void ControlPanelForm::j2stop_handler()
{
  emit stopJoint(2);
}

void ControlPanelForm::setSpeed1Label(double value)
{
  this->edit_speed1->clear();
  this->edit_speed1->setText(QString::number(value));
}

void ControlPanelForm::setSpeed1Label(int value)
{
  this->edit_speed1->clear();
  this->edit_speed1->setText(QString::number(value));
}

void ControlPanelForm::speed1EntryActivated()
{
  QString text = this->edit_speed1->text();
  this->slider_speed1->setValue(text.toInt());
}

void ControlPanelForm::setSpeed2Label(double value)
{
  this->edit_speed2->clear();
  this->edit_speed2->setText(QString::number(value));
}

void ControlPanelForm::setSpeed2Label(int value)
{
  this->edit_speed2->clear();
  this->edit_speed2->setText(QString::number(value));
}

void ControlPanelForm::speed2EntryActivated()
{
  QString text = this->edit_speed2->text();
  this->slider_speed2->setValue(text.toInt());
}

void ControlPanelForm::setActiveRobot(int index)
{
  /* Get the mobot object */
  RecordMobot *mobot;
  mobot = robotManager()->getMobotIndex(index);
  if(mobot != NULL && mobot->isConnected()) {
    this->setEnabled(true);
    asyncrobot_->bindMobot(mobot);
    asyncrobot_->enableJointSignals(true);
    asyncrobot_->enableAccelSignals(true);
    QMetaObject::invokeMethod(asyncrobot_, "startWork", Qt::QueuedConnection);
    emit setUIWidgetsState(true);
  } else {
    /* Stop the thread if it is running */
    asyncrobot_->stopWork();
    this->setEnabled(false);
    emit setUIWidgetsState(false);
  }
}

void ControlPanelForm::setActiveRobot(const QModelIndex &index)
{
  setActiveRobot(index.row());
}
