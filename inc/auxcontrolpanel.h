#ifndef AUXCONTROLPANEL_H__
#define AUXCONTROLPANEL_H__

#include "ui_auxcontrol.h"
#include "asyncrobot.h"

class AuxControlPanelForm : public QWidget, public Ui::auxcontrol
{
  Q_OBJECT
  public:
    AuxControlPanelForm(AsyncRobot* asyncRobot, QWidget * parent = 0);

  public slots:
    void beepPressed();
    void beepReleased();
    void buzzerSliderChanged(int value);
    void buzzerTextChanged(const QString & text);
    void colorButtonPressed();
    void handleBuzzerCheckbox(int state);
    void setAccelEnable(int state);
    void setAccelSliders(double x, double y, double z);

  private:
    AsyncRobot *asyncrobot_;
};

#endif
