#include "auxcontrolpanel.h"
#include "asyncrobot.h"
#include <QColorDialog>

AuxControlPanelForm::AuxControlPanelForm(AsyncRobot* asyncRobot, QWidget *parent)
  : QWidget(parent)
{
  setupUi(this);
  asyncrobot_ = asyncRobot;
  /* Connect buttons and widgets */
  QObject::connect(pushButton_beep, SIGNAL(pressed()), 
      this, SLOT(beepPressed()));
  QObject::connect(pushButton_beep, SIGNAL(released()), 
      this, SLOT(beepReleased()));
  QObject::connect(checkBox_buzzerOn, SIGNAL(stateChanged(int)),
      this, SLOT(handleBuzzerCheckbox(int)));
  QObject::connect(horizontalSlider_buzzerFreq, SIGNAL(sliderMoved(int)),
      this, SLOT(buzzerSliderChanged(int)));
  QObject::connect(lineEdit_buzzerFreq, SIGNAL(textEdited(const QString &)),
      this, SLOT(buzzerTextChanged(const QString &)));
  QObject::connect(checkBox_enableAccel, SIGNAL(stateChanged(int)),
      this, SLOT(setAccelEnable(int)));
  QObject::connect(pushButton_ledcolor, SIGNAL(clicked()), 
      this, SLOT(colorButtonPressed()));

  /* Connect external signals to this widget */
  QObject::connect(asyncrobot_, SIGNAL(accelChanged(double, double, double)),
      this, SLOT(setAccelSliders(double, double, double)));
  setEnabled(false);
}

void AuxControlPanelForm::beepPressed()
{
  /* Get the frequency from the line edit */
  QString text;
  text = this->lineEdit_buzzerFreq->displayText();
  asyncrobot_->setBuzzerFrequency(text.toInt());
  asyncrobot_->setBuzzerOn();
}

void AuxControlPanelForm::beepReleased()
{
  asyncrobot_->setBuzzerOff();
}

void AuxControlPanelForm::buzzerSliderChanged(int value)
{
  lineEdit_buzzerFreq->setText(QString::number(value));
  asyncrobot_->setBuzzerFrequency(value);
}

void AuxControlPanelForm::buzzerTextChanged(const QString & text)
{
  horizontalSlider_buzzerFreq->setValue(text.toInt());
  asyncrobot_->setBuzzerFrequency(text.toInt());
}

void AuxControlPanelForm::colorButtonPressed()
{
  static QColorDialog *d = NULL;
  if(d == NULL) {
    d = new QColorDialog(this);
    QObject::connect(d, SIGNAL(currentColorChanged(const QColor &)),
        asyncrobot_, SLOT(setLEDColor(const QColor &)));
  }
  QColor color = asyncrobot_->getLEDColor();
  d->setCurrentColor(color);
  d->show();
  d->raise();
  d->activateWindow();
}

void AuxControlPanelForm::handleBuzzerCheckbox(int state)
{
  if(state) {
    /* Turn off beep button */
    pushButton_beep->setEnabled(false);
    /* Start buzzing the buzzer */
    asyncrobot_->setBuzzerOn();
  } else {
    pushButton_beep->setEnabled(true);
    asyncrobot_->setBuzzerOff();
  }
}

void AuxControlPanelForm::setAccelEnable(int state)
{
  if(state) {
    asyncrobot_->enableAccelSignals();
    lineEdit_xaccel->setEnabled(true);
    lineEdit_yaccel->setEnabled(true);
    lineEdit_zaccel->setEnabled(true);
    lineEdit_magaccel->setEnabled(true);
    verticalSlider_xaccel->setEnabled(true);
    verticalSlider_yaccel->setEnabled(true);
    verticalSlider_zaccel->setEnabled(true);
    verticalSlider_magaccel->setEnabled(true);
  } else {
    asyncrobot_->disableAccelSignals();
    lineEdit_xaccel->setEnabled(false);
    lineEdit_yaccel->setEnabled(false);
    lineEdit_zaccel->setEnabled(false);
    lineEdit_magaccel->setEnabled(false);
    verticalSlider_xaccel->setEnabled(false);
    verticalSlider_yaccel->setEnabled(false);
    verticalSlider_zaccel->setEnabled(false);
    verticalSlider_magaccel->setEnabled(false);
  }
}

void AuxControlPanelForm::setAccelSliders(double x, double y, double z)
{
  verticalSlider_xaccel->setValue(x*100);
  verticalSlider_yaccel->setValue(y*100);
  verticalSlider_zaccel->setValue(z*100);
  verticalSlider_magaccel->setValue( sqrt(x*x + y*y + z*z) * 100 );
  lineEdit_xaccel->setText( QString::number(x) );
  lineEdit_yaccel->setText( QString::number(y) );
  lineEdit_zaccel->setText( QString::number(z) );
  lineEdit_magaccel->setText( QString::number(sqrt(x*x + y*y + z*z)) );
}

