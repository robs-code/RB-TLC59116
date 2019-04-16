#include "RB-TLC59116.h"

const int TLC_ADDR = 0x60;
const byte RESET_ADDR = 0x6B;
const byte EFLAG1_ADDR = 0x1D;
const byte EFLAG2_ADDR = 0x1E;
const byte LEDOUT_ADDR = 0x14;
const byte PWM_ADDR = 0x02;
const byte PWM_GROUP_ADDR = 0x12;
const byte FREQ_GROUP_ADDR = 0x12;

const byte LED_OFF = 0x00;
const byte LED_ON = 0x01;
const byte LED_PWM = 0x02;
const byte LED_GROUP = 0x03;

boolean groupMode = 0; // Group dimming(0) or blinking(1)
byte LEDOUT[4] = { 0x00, 0x00, 0x00, 0x00 }; // Keep track of channel output modes

//<<constructor>> setup the TLC59116
TLC59116::TLC59116() {}

//<<destructor>>
TLC59116::~TLC59116() {}

void TLC59116::write2Register(byte reg, byte val)
{
  Wire.beginTransmission(TLC_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

byte TLC59116::readFromRegister(byte reg)
{
  uint8_t result;
  Wire.beginTransmission(TLC_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(TLC_ADDR, 1);
  while (Wire.available()) result = Wire.read();
  return result;
}

// Datasheet 9.5.6
// --------------------------------------------------------------------
// When modifying a channel output, maintain all other channel outputs.
byte TLC59116::modLEDOUT(byte pos, byte regLoc, byte mod) {
  byte mask = 0x03 << pos;
  byte val;

  val = LEDOUT[regLoc] | mask;
  val = LEDOUT[regLoc] & ~mask;
  if (mod != 0x00) val = LEDOUT[regLoc] | (mod << pos);
  LEDOUT[regLoc] = val;

  return val;
}

void TLC59116::channelOff(byte channel) {
  byte pos = ((channel % 4) * 2);
  byte regLoc = channel / 4;

  write2Register(LEDOUT_ADDR + regLoc, modLEDOUT(pos, regLoc, LED_OFF));
}

void TLC59116::channelOn(byte channel) {
  byte pos = ((channel % 4) * 2);
  byte regLoc = channel / 4;

  write2Register(LEDOUT_ADDR + regLoc, modLEDOUT(pos, regLoc, LED_ON));
}

void TLC59116::channelPWM(byte channel) {
  byte pos = ((channel % 4) * 2);
  byte regLoc = channel / 4;

  setPWM(channel, 255);
  write2Register(LEDOUT_ADDR + regLoc, modLEDOUT(pos, regLoc, LED_PWM));
}

void TLC59116::channelGroup(byte channel) {
  byte pos = ((channel % 4) * 2);
  byte regLoc = channel / 4;

  setPWM(channel, 255);
  write2Register(LEDOUT_ADDR + regLoc, modLEDOUT(pos, regLoc, LED_GROUP));
}
// --------------------------------------------------------------------

// Datasheet 9.5.3
void TLC59116::setPWM(byte pin, byte duty) {
  write2Register(PWM_ADDR + pin, duty);
}

// Datasheet 9.5.4
void TLC59116::setGroupPWM(byte duty) {
  if (groupMode == 1) {
    write2Register(0x01, 0x00);
    groupMode = 0;
  }
  write2Register(PWM_GROUP_ADDR, duty);
}

void TLC59116::setGroupFreq(byte freq) {
  if (groupMode == 0) {
    write2Register(0x01, 0x20);
    groupMode = 1;
  }

  write2Register(FREQ_GROUP_ADDR, freq);
}

// Datasheet 9.5.2
void TLC59116::clearErrors() {
  write2Register(0x01, 0x80);
  write2Register(0x01, 0x00);
}

// Datasheet 9.3.2 & 9.5.10
unsigned int TLC59116::checkErrors(bool report) {
  byte flag1 = readFromRegister(EFLAG1_ADDR);
  byte flag2 = readFromRegister(EFLAG2_ADDR);
  unsigned int flag = (flag2 << 8) + flag1;

  if (report) {
    if (flag == 0xFFFF)Serial.println("No errors detected.");
    else {
      for (byte i = 0; i < 16; i++) {
        if (!((flag >> i) & 0x1)) {
          Serial.print("Channel ");
          Serial.print(i);
          Serial.println(" overtemperature or open circuit.");
        }
      }
    }
  }

  return flag;
}

void TLC59116::enableTLC() {
  write2Register(0x00, 0x80);

  //Reset Error Flags
  clearErrors();
}

// Datasheet 9.3.5
void TLC59116::softReset()
{
  for (byte i = 0; i < 4; i++) LEDOUT[i] = 0x00;

  Wire.beginTransmission(0x6B);
  Wire.write(0xA5);
  Wire.write(0x5A);
  Wire.endTransmission();

  enableTLC();
}
