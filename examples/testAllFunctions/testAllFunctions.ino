#include <RB-TLC59116.h>

TLC59116 tlc;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  while (!Serial); // Leonardo: wait for serial monitor
  
  tlc.softReset(); // Also enables at the end
 }

void loop() {
  testTLC();
}

void testTLC(){
  // Test Channel On and Error Check
  Serial.println("Testing channels and error reporting");
  for(byte i=0; i<16; i++){
    tlc.channelOn(i);
    delay(100);
  }
  tlc.checkErrors(true);
  tlc.clearErrors();
  for(byte i=0; i<16; i++){
    tlc.channelOff(i);
    delay(100);
  }
  tlc.softReset();
  
  // Test Single PWM
  Serial.println("Testing single channel PWM");
  tlc.channelPWM(0);
  tlc.channelPWM(1);
  for(int i=0; i<256; i++){
    tlc.setPWM(0, i);
    tlc.setPWM(1,255-i);
    delay(5);
  }
  for(int i=255; i>0; i--){
    tlc.setPWM(0, i);
    tlc.setPWM(1,255-i);
    delay(5);
  }
  for(int i=0; i<256; i++){
    tlc.setPWM(0, i);
    tlc.setPWM(1,255-i);
    delay(5);
  }
  for(int i=255; i>0; i--){
    tlc.setPWM(0, i);
    tlc.setPWM(1,255-i);
    delay(5);
  }
  tlc.softReset();
  
  // Test Group PWM
  Serial.println("Testing group PWM and Blinking");
  for(byte i=0; i<16; i+=2) tlc.channelGroup(i);
  for(int i=0; i<256; i++){
    tlc.setGroupPWM(i);
    delay(5);
  }
  for(int i=255; i>0; i--){
    tlc.setGroupPWM(i);
    delay(5);
  }
  for(int i=0; i<256; i++){
    tlc.setGroupFreq(i);
    delay(20);
  }
  for(int i=255; i>0; i--){
    tlc.setGroupFreq(i);
    delay(20);
  }
  tlc.softReset();
}
