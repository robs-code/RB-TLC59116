#include "RB-TLC59116.h"

// Register Addresses
const byte TLC_PWM_BASE = 0x02;
const byte TLC_GROUP_PWM = 0x12;
const byte TLC_GROUP_FREQ = 0x13;
const byte TLC_LED_OUTPUT_ADDR = 0x14;
const byte TLC_ERROR_FLAG1 = 0x1D;
const byte TLC_ERROR_FLAG2 = 0x1E;
const byte TLC_RESET = 0x6B;

// Possible LED Output States
const byte LED_OFF = 0x00;
const byte LED_ON = 0x01;
const byte LED_PWM = 0x02;
const byte LED_GROUP = 0x03;

//<<constructor>> setup the TLC59116
TLC59116::TLC59116(int address, byte resetPin) : TLC_ADDR(address), RESET(resetPin) {
  digitalWrite(RESET, HIGH);
  boolean groupMode = 0; // Group dimming(0) or blinking(1)
  byte LEDOUT[4] = { 0x00, 0x00, 0x00, 0x00 }; // Keep track of channel output modes
  unsigned int ERR_FLAG = 0xFFFF;
}
  
//<<destructor>>
TLC59116::~TLC59116() {}

void TLC59116::writeToDevice(byte reg, byte newLEDOUT)
{
  Wire.beginTransmission(TLC_ADDR);
  Wire.write(reg);
  Wire.write(newLEDOUT);
  Wire.endTransmission();
}

byte TLC59116::readFromDevice(byte reg)
{
  Wire.beginTransmission(TLC_ADDR);
  Wire.write(reg);
  Wire.requestFrom(TLC_ADDR, 1);
  uint8_t result = Wire.read();
  Wire.endTransmission();
  return result;
}

// Datasheet 9.5.6
// --------------------------------------------------------------------
// When modifying a channel output, maintain all other channel outputs.
byte TLC59116::modifyLEDOutputState(byte LED, byte state) {
  byte pos = ((LED % 4) * 2);
  byte regLoc = LED / 4;
  byte mask = 0x03 << pos;
  byte newLEDOUT;

  newLEDOUT = LEDOUT[regLoc] | mask;
  newLEDOUT = LEDOUT[regLoc] & ~mask;
  if (state != 0x00) newLEDOUT = LEDOUT[regLoc] | (state << pos);
  LEDOUT[regLoc] = newLEDOUT;

  writeToDevice(TLC_LED_OUTPUT_ADDR + regLoc, newLEDOUT);
}

void TLC59116::LEDOff(byte LED) {
  modifyLEDOutputState(LED, LED_OFF);
}

void TLC59116::LEDOn(byte LED) {
  modifyLEDOutputState(LED, LED_ON);
}

void TLC59116::LEDPWM(byte LED) {
  setPWM(LED, 255);
  modifyLEDOutputState(LED, LED_PWM);
}

void TLC59116::LEDGroup(byte LED) {
  setPWM(LED, 255);
  modifyLEDOutputState(LED, LED_GROUP);
}
// --------------------------------------------------------------------

// Datasheet 9.5.3
void TLC59116::setPWM(byte pin, byte duty) {
  writeToDevice(TLC_PWM_BASE + pin, duty);
}

// Datasheet 9.5.4
void TLC59116::setGroupPWM(byte duty) {
  if (groupMode == 1) {
    writeToDevice(0x01, 0x00);
    groupMode = 0;
  }
  writeToDevice(TLC_GROUP_PWM, duty);
}

void TLC59116::setGroupBlink(byte freq, byte duty) {
  setGroupPWM(duty);
  if (groupMode == 0) {
    writeToDevice(0x01, 0x20);
    groupMode = 1;
  }

  writeToDevice(TLC_GROUP_FREQ, freq);
}

// Datasheet 9.5.2
void TLC59116::clearErrors() {
  writeToDevice(0x01, 0x80);
  writeToDevice(0x01, 0x00);
}

// Datasheet 9.3.2 & 9.5.10
boolean TLC59116::checkErrors() {
  byte flag1 = readFromDevice(TLC_ERROR_FLAG1);
  byte flag2 = readFromDevice(TLC_ERROR_FLAG2);
  Serial.println(flag1);
  Serial.println(flag2);
  ERR_FLAG = (flag2 << 8) + flag1;

  if (ERR_FLAG == 0xFFFF) return false;
  return true;
   
}

void TLC59116::reportErrors(){
  if (ERR_FLAG == 0xFFFF) Serial.println("No errors detected.");
  else {
    Serial.println(ERR_FLAG, HEX);
   for (byte i = 0; i < 16; i++) {
    if (!((ERR_FLAG >> i) & 0x1)) {
      Serial.print("Channel ");
      Serial.print(i);
      Serial.println(" overtemperature or open circuit.");
      }
    }
  }
  clearErrors();
}

// Start up the driver in a fresh state
void TLC59116::enableTLC() {
  writeToDevice(0x00, 0x80);
}

// Reset the driver either by the RESET pin, and if it was not defined just turn off all the LEDs.
void TLC59116::resetDriver(){
  for(int i=0; i<4; i++){
    LEDOUT[i] = 0x00;
  }

  digitalWrite(RESET, LOW);
  delay(1); // Allow a microsecond for the device to power down.
  digitalWrite(RESET, HIGH);
  enableTLC();
}

// Nice function to turn off all LED channels.
void TLC59116::turnOffAllLEDs(){
  for(byte i=0; i<16; i++){
    LEDOff(i);
  }
}

// Datasheet 9.3.5
void TLC59116::resetAllTLCs()
{
  for (byte i = 0; i < 4; i++) LEDOUT[i] = 0x00;

  Wire.beginTransmission(0x6B);
  Wire.write(0xA5);
  Wire.write(0x5A);
  Wire.endTransmission();

  enableTLC();
}
