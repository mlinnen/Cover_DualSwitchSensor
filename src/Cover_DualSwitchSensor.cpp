#include "Arduino.h"
#include "Cover_DualSwitchSensor.h"

namespace Cover
{
  DualSwitchSensor::DualSwitchSensor(int downSensorPin, int upSensorPin)
  {
    _closeSensor = new RBD::Button(downSensorPin);
    _closeSensor->setDebounceTimeout(500);
    _openSensor = new RBD::Button(upSensorPin);
    _openSensor->setDebounceTimeout(500);

  }

  void DualSwitchSensor::setup()
  {
  }

  boolean DualSwitchSensor::loop()
  {
    // This is the service loop that is called from the main program and will update the state of this component.

    // Reset the state changed indicator
    _stateChanged = false;

    // Read the sensors
    if (_currentState==StatePowerUp)
    { 
      if (_closeSensor->isPressed() && !_openSensor->isPressed())
        _currentState = StateClosed;
      else if (!_closeSensor->isPressed() && _openSensor->isPressed())
        _currentState = StateOpen;
      else
        _currentState = StateUnknown;
      // Clear the onPressed and onReleased states
      _closeSensor->onPressed();
      _openSensor->onPressed();
      _closeSensor->onReleased();
      _openSensor->onReleased();
    }
    else
    {
      boolean closedPressed = _closeSensor->onPressed();
      boolean closedOnReleased = _closeSensor->onReleased();
      boolean openPressed = _openSensor->onPressed();
      boolean openOnReleased = _openSensor->onReleased();
      if(closedPressed) {
        Serial.println("Close Sensor Activate");
        _currentState = StateClosed;
      }
      if(closedOnReleased) {
        Serial.println("Close Sensor Deactivated");
        _currentState = StateOpening;
      }
      if (openPressed && (_lastState == StateUnknown)) {
        Serial.println("Open Sensor Activated");
        _currentState = StateOpen;
      }
      else if(openPressed && (_lastState == StateOpening)) {
        Serial.println("Open Sensor Activated");
        _currentState = StateOpen;
      }
      else if(openPressed && (_lastState == StateClosing)) {
        Serial.println("Open Sensor Activated");
        _currentState = StateOpen;
        // This is an error state since the cover was closing then it opened
      }
      if(openOnReleased) {
        Serial.println("Open Sensor Deactivated");
        _currentState = StateClosing;
      }
    }

    // Set the State Changed indicator
    if (_lastState != _currentState)
      _stateChanged = true;

    // Save off the current state as the last state
    _lastState = _currentState;

    // TODO Determine if we have hit a timeout
  }

  boolean DualSwitchSensor::getStateChanged()
  {
    return _stateChanged;
  }

  State DualSwitchSensor::getCurrentState()
  {
    return _currentState;
  }
}
