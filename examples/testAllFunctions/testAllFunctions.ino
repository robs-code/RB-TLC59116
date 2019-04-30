#include <RB-TLC59116.h>

TLC59116 tlc(0x61, 11);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  while (!Serial); // Leonardo: wait for serial monitor

  tlc.enableTLC();
  tlc.resetDriver(); // Also enables at the end
 }

void loop() {
  testTLC();
}

void testTLC(){
  // Test Channel On and Error Check
  Serial.println("Testing channels and error reporting");
  for(byte i=0; i<16; i++){
    tlc.LEDOn(i);
    delay(100);
  }
  if(tlc.checkErrors()) tlc.reportErrors();
  for(byte i=0; i<16; i++){
    tlc.LEDOff(i);
    delay(100);
  }
  tlc.resetDriver();
  
  // Test Single PWM
  Serial.println("Testing single channel PWM");
  tlc.LEDPWM(0);
  tlc.LEDPWM(1);
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
  tlc.resetDriver();
  
  // Test Group PWM
  Serial.println("Testing group PWM and Blinking");
  for(byte i=0; i<16; i+=2) tlc.LEDGroup(i);
  for(int i=0; i<256; i++){
    tlc.setGroupPWM(i);
    delay(5);
  }
  for(int i=255; i>0; i--){
    tlc.setGroupPWM(i);
    delay(5);
  }
    
  tlc.setGroupBlink(0, 128);
  delay(1000);
  tlc.setGroupBlink(47, 128);
  delay(4000);
  tlc.resetDriver();
}
