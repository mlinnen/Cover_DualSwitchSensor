#ifndef CoverDualSwitchSensor_h
#define CoverDualSwitchSensor_h

#include "Arduino.h"
#include <RBD_Button.h> 
#include "Cover_Core.h"

namespace Cover
{
  class DualSwitchSensor
  {
  public:
    DualSwitchSensor(int downSensorPin, int upSensorPin);
    void setup();
    boolean loop();
    boolean getStateChanged();
    State getCurrentState();

  private:
    RBD::Button* _openSensor;
    RBD::Button* _closeSensor;
    State _lastState = StatePowerUp;
    State _currentState = StatePowerUp;
    boolean _stateChanged = false;
    boolean _powerUp = true;
  };
}

#endif