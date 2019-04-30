#pragma once

#ifndef TLC59116_H
#define TLC59116_H

#include <arduino.h>
#include <Wire.h>

class TLC59116 {
  int TLC_ADDR;
  byte RESET;
  
  public:
  	TLC59116(int address, byte resetPin);
  	~TLC59116();
  
  	void LEDOff(byte LED);
  	void LEDOn(byte LED);
  	void LEDPWM(byte LED);
  	void LEDGroup(byte LED);
  	void setPWM(byte LED, byte duty);
  	void setGroupPWM(byte duty);
  	void setGroupBlink(byte freq, byte duty);
  	void clearErrors();
  	boolean checkErrors();
    void reportErrors();
  	void enableTLC();
    void resetDriver();
    void turnOffAllLEDs();
  	void resetAllTLCs();
  
  private:
  	void writeToDevice(byte reg, byte val);
  	byte readFromDevice(byte reg);
  	byte modifyLEDOutputState(byte LED, byte state);    
    boolean groupMode;
    byte LEDOUT[4];
    unsigned int ERR_FLAG;
};

#endif // !TLC59116_H
