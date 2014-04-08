#ifndef ASYNCMOBOT_H_
#define ASYNCMOBOT_H_

#include <QColor>
#include <QObject>
#include <QMutex>
#include <QTimer>
#include <linkbot.h>

class AsyncRobot:public QObject
{
  Q_OBJECT
  public:
    AsyncRobot();
    ~AsyncRobot();
    void bindMobot(CLinkbot* mobot);
    QColor getLEDColor();

  public slots:
    void acquireJointControl();
    void driveJointTo(int joint, double angle);
    void doWork(); // Worker thread
    void disableJointSignals();
    void disableAccelSignals();
    void enableAccelSignals(bool enable = true);
    void enableAccelSignals(int enable);
    void enableJointSignals(bool enable = true);
    void enableJointSignals(int enable);
    void moveBackward();
    void moveForward();
    void moveJoint(int joint, int direction);
    void move(double angle1, double angle2, double angle3);
    void releaseJointControl();
    void resetToZero();
    void setBuzzerFrequency(int freq);
    void setBuzzerOn();
    void setBuzzerOff();
    void setLEDColor(const QColor & color);
    void setSpeed1(int speed);
    void setSpeed2(int speed);
    void setState(int state);
    void startWork();
    void stop();
    void stopJoint(int joint);
    void stopWork();
    void turnLeft();
    void turnRight();

  signals:
    void accelChanged(double x, double y, double z);
    void jointAnglesChanged(double angle1, double angle2, double angle3);
    void joint1Changed(double angle);
    void joint2Changed(double angle);
    void joint1Changed(int angle);
    void joint2Changed(int angle);
    void speed1Changed(double speed);
    void speed1Changed(int speed);
    void speed2Changed(double speed);
    void speed2Changed(int speed);

  private:
    CLinkbot* mobot_;
    bool jointSignalEnable_;
    bool accelSignalEnable_;
    QMutex mobotLock_; // Protects 3 above members

    double lastJointAngles_[3];
    double lastAccel_[3];
    double desiredJointAngles_[3];
    unsigned int anglesDirtyMask_;
    QMutex desiredJointAnglesLock_;
    bool jointControl_;
    QTimer *timer_;
    bool buzzerState_;
    int buzzerFreq_;
};

#endif
