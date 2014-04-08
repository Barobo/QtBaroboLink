#include "asyncrobot.h"
#include "mainwindow.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

AsyncRobot::AsyncRobot() 
{
  jointSignalEnable_ = false;
  accelSignalEnable_ = false;
  buzzerState_ = false;
  buzzerFreq_ = 440;
  timer_ = new QTimer(this);
  jointControl_ = false;
}

AsyncRobot::~AsyncRobot() {}

void AsyncRobot::acquireJointControl()
{
  qDebug() << "Joint control acquired.";
  jointControl_ = true;
}

void AsyncRobot::bindMobot(CLinkbot* mobot)
{
  mobotLock_.lock();
  mobot_ = mobot;
  anglesDirtyMask_ = 0;
  mobotLock_.unlock();
}

QColor AsyncRobot::getLEDColor()
{
  int r, g, b;
  mobotLock_.lock();
  if(mobot_ == NULL) {
    mobotLock_.unlock();
    return QColor(0, 0, 0);
  }
  mobot_->getColorRGB(r, g, b);
  mobotLock_.unlock();
  return QColor(r, g, b);
}

void AsyncRobot::disableJointSignals()
{
  qDebug() << "Joint Signals Disabled.";
  enableJointSignals(false);
}

void AsyncRobot::disableAccelSignals()
{
  enableAccelSignals(false);
}

void AsyncRobot::doWork()
{
  double jointAngles[3];
  double accel[3];
  mobotLock_.lock();
  if(
      (mobot_ == NULL)/* ||
      (!mobot_->isConnected()) */
    ) {
    mobotLock_.unlock();
    return;
  }
  if(
      jointSignalEnable_ &&
      (mainWindow()->currentTab() == 0)
    ) {
    mobot_->getJointAngles( 
        jointAngles[0], 
        jointAngles[1], 
        jointAngles[2]);
    if(memcmp(jointAngles, lastJointAngles_, sizeof(double)*3)) {
      emit jointAnglesChanged(jointAngles[0], jointAngles[1], jointAngles[2]);
      double norm;
      if(jointAngles[0] != lastJointAngles_[0]) {
        emit joint1Changed(((int)-jointAngles[0])%360);
        norm = fmod(jointAngles[0], 360);
        if(norm > 180.0) {
          norm -= 360.0;
        }
        emit joint1Changed(norm);
      }
      if(jointAngles[1] != lastJointAngles_[1]) {
        /* Negative angles because the directionality of the Qt dials is
         * opposite of our joint directions. */
        emit joint2Changed(((int)-jointAngles[1])%360);
        norm = fmod(jointAngles[1], 360);
        if(norm > 180.0) {
          norm -= 360.0;
        }
        emit joint2Changed(norm);
      }
      if(jointAngles[2] != lastJointAngles_[2]) {
        emit joint2Changed(((int)-jointAngles[2])%360);
        norm = fmod(jointAngles[2], 360);
        if(norm > 180.0) {
          norm -= 360.0;
        }
        emit joint2Changed(norm);
      }
      memcpy(lastJointAngles_, jointAngles, sizeof(double)*3);
    }
  }
  if(
      accelSignalEnable_ &&
      (mainWindow()->currentTab() == 1)
      ) {
    mobot_->getAccelerometerData(accel[0], accel[1], accel[2]);
    if(memcmp(accel, lastAccel_, sizeof(double)*3)) {
      emit accelChanged(accel[0], accel[1], accel[2]);
      memcpy(lastAccel_, accel, sizeof(double)*3);
    }
  }

  desiredJointAnglesLock_.lock();
  int i;
  for(i = 0; i < 4; i++) {
    if(anglesDirtyMask_ & (1<<i)) {
      mobot_->driveJointToDirectNB( (robotJointId_t)(i+1), desiredJointAngles_[i]);
    }
  }
  anglesDirtyMask_ = 0;
  desiredJointAnglesLock_.unlock();

  mobotLock_.unlock();
  QThread::yieldCurrentThread();
}

void AsyncRobot::driveJointTo(int joint, double angle)
{
  if(false == jointControl_) return;
  qDebug() << "Drive joint " << joint << " to " << angle;
  desiredJointAnglesLock_.lock();
  desiredJointAngles_[joint-1] = angle;
  anglesDirtyMask_ |= 1<<(joint-1);
  desiredJointAnglesLock_.unlock();
}

void AsyncRobot::enableJointSignals(bool enable)
{
  qDebug() << "enable Joint Signals: " << enable;
  mobotLock_.lock();
  jointSignalEnable_ = enable;
  mobotLock_.unlock();
}

void AsyncRobot::enableJointSignals(int enable)
{
  enableJointSignals(static_cast<bool>(enable));
}

void AsyncRobot::enableAccelSignals(bool enable)
{
  mobotLock_.lock();
  accelSignalEnable_ = enable;
  mobotLock_.unlock();
}

void AsyncRobot::enableAccelSignals(int enable)
{
  enableJointSignals(static_cast<bool>(enable));
}

void AsyncRobot::moveBackward()
{
  mobotLock_.lock();
  mobot_->moveContinuousNB(
      ROBOT_BACKWARD,
      ROBOT_BACKWARD,
      ROBOT_BACKWARD);
  mobotLock_.unlock();
}

void AsyncRobot::moveForward()
{
  mobotLock_.lock();
  mobot_->moveContinuousNB(
      ROBOT_FORWARD,
      ROBOT_FORWARD,
      ROBOT_FORWARD);
  mobotLock_.unlock();
}

void AsyncRobot::moveJoint(int joint, int direction)
{
  mobotLock_.lock();
  mobot_->setJointMovementStateNB(
      static_cast<robotJointId_t>(joint), 
      static_cast<robotJointState_t>(direction));
  if(joint == 2) {
  mobot_->setJointMovementStateNB(
      static_cast<robotJointId_t>(3), 
      static_cast<robotJointState_t>(direction));
  }
  mobotLock_.unlock();
}

void AsyncRobot::move(double j1, double j2, double j3)
{
  mobotLock_.lock();
  mobot_->moveNB(j1, j2, j3);
  mobotLock_.unlock();
}

void AsyncRobot::releaseJointControl()
{
  qDebug() << "Joint control released.";
  jointControl_ = false;
}

void AsyncRobot::resetToZero()
{
  mobotLock_.lock();
  mobot_->resetToZeroNB();
  mobotLock_.unlock();
}

void AsyncRobot::setBuzzerFrequency(int freq)
{
  mobotLock_.lock();
  buzzerFreq_ = freq;
  if(buzzerState_) {
    if(mobot_ == NULL) {
      mobotLock_.unlock();
      return;
    }
    mobot_->setBuzzerFrequencyOn(freq);
  }
  mobotLock_.unlock();
}

void AsyncRobot::setBuzzerOn()
{
  mobotLock_.lock();
  buzzerState_ = true;
  if(mobot_ == NULL) {
    mobotLock_.unlock();
    return;
  }
  mobot_->setBuzzerFrequencyOn(buzzerFreq_);
  mobotLock_.unlock();
}

void AsyncRobot::setBuzzerOff()
{
  mobotLock_.lock();
  buzzerState_ = false;
  if(mobot_ == NULL) {
    mobotLock_.unlock();
    return;
  }
  mobot_->setBuzzerFrequencyOn(0);
  mobotLock_.unlock();
}

void AsyncRobot::setLEDColor(const QColor &color)
{
  mobotLock_.lock();
  if(mobot_ == NULL) {
    mobotLock_.unlock();
    return;
  }
  mobot_->setColorRGB(color.red(), color.green(), color.blue());
  mobotLock_.unlock();
}

void AsyncRobot::setSpeed1(int speed)
{
  mobotLock_.lock();
  mobot_->setJointSpeed((robotJointId_t)1, speed);
  mobotLock_.unlock();
}

void AsyncRobot::setSpeed2(int speed)
{
  mobotLock_.lock();
  mobot_->setJointSpeed((robotJointId_t)2, speed);
  mobot_->setJointSpeed((robotJointId_t)3, speed);
  mobotLock_.unlock();
}

void AsyncRobot::setState(int state)
{
  if(state) {
    enableJointSignals(true);
    enableAccelSignals(true);
  } else {
    enableJointSignals(false);
    enableAccelSignals(false);
  }
}

void AsyncRobot::startWork()
{
  mobotLock_.lock();
  if(timer_->isActive()) {
    mobotLock_.unlock();
    return;
  }
  if(
      (mobot_ != NULL) &&
      (mobot_->isConnected())
    ) {
    /* Get the motor speeds */
    double speeds[3];
    int formFactor;
    mobot_->getFormFactor(formFactor);
    mobot_->getJointSpeeds(speeds[0], speeds[1], speeds[2]);
    if(formFactor == MOBOTFORM_I) {
      emit speed1Changed(speeds[0]);
      emit speed1Changed((int)speeds[0]);
      emit speed2Changed(speeds[2]);
      emit speed2Changed((int)speeds[2]);
    } else if (formFactor == MOBOTFORM_L) {
      emit speed1Changed(speeds[0]);
      emit speed1Changed((int)speeds[0]);
      emit speed2Changed(speeds[1]);
      emit speed2Changed((int)speeds[1]);
    }
  }
  anglesDirtyMask_ = 0;
  connect(timer_, SIGNAL(timeout()), this, SLOT(doWork()));
  timer_->start();
  mobotLock_.unlock();
}

void AsyncRobot::stop()
{
  mobotLock_.lock();
  mobot_->stop();
  mobotLock_.unlock();
}

void AsyncRobot::stopJoint(int joint)
{
  mobotLock_.lock();
  mobot_->stopOneJoint(static_cast<robotJointId_t>(joint));
  if(joint == 2) {
    mobot_->stopOneJoint(static_cast<robotJointId_t>(3));
  }
  mobotLock_.unlock();
}

void AsyncRobot::stopWork()
{
  mobotLock_.lock();
  timer_->stop();
  mobotLock_.unlock();
}

void AsyncRobot::turnLeft()
{
  mobotLock_.lock();
  mobot_->moveContinuousNB(
      ROBOT_BACKWARD,
      ROBOT_BACKWARD,
      ROBOT_FORWARD);
  mobotLock_.unlock();
}

void AsyncRobot::turnRight()
{
  mobotLock_.lock();
  mobot_->moveContinuousNB(
      ROBOT_FORWARD,
      ROBOT_BACKWARD,
      ROBOT_BACKWARD);
  mobotLock_.unlock();
}

