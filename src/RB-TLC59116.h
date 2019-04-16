#pragma once

#ifndef TLC59116_H
#define TLC59116_H

#include <arduino.h>
#include <Wire.h>

class TLC59116 {
public:
	TLC59116();
	~TLC59116();

	void channelOff(byte channel);
	void channelOn(byte channel);
	void channelPWM(byte channel);
	void channelGroup(byte channel);
	void setPWM(byte channel, byte duty);
	void setGroupPWM(byte duty);
	void setGroupFreq(byte freq);
	void clearErrors();
	unsigned int checkErrors(bool report);
	void enableTLC();
	void softReset();
	//void stateChange(bool opt);

private:
	void write2Register(byte reg, byte val);
	byte readFromRegister(byte reg);
	byte modLEDOUT(byte pos, byte regLoc, byte mod);
};

#endif // !TLC59116_H
