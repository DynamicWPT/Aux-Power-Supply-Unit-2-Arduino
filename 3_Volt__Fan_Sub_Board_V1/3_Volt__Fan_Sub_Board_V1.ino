#include <Wire.h>

#define slaveADDR 0x28
#define SWTICH_24V_PIN 4

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(); 
  Wire.setClock(1000);
  pinMode(SWTICH_24V_PIN, OUTPUT);
  digitalWrite(SWTICH_24V_PIN, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("ok");
  cursorHome();
  //writeString("Data");
  delay(1000);

}

void writeString(char* data) {
  Wire.beginTransmission(slaveADDR); // transmit to device #4
  Wire.write(data);
  Wire.endTransmission();    // stop transmitting
  delayMicroseconds(100);
}

void displayOn() {
  Wire.beginTransmission(slaveADDR); // transmit to device #4
  Wire.write(0xFE);  
  Wire.write(0x41);                   
  Wire.endTransmission();    // stop transmitting
  delayMicroseconds(100);
}

void displayOff() {
  Wire.beginTransmission(slaveADDR); // transmit to device #4
  Wire.write(0xFE);
  Wire.write(0x42);                   
  Wire.endTransmission();    // stop transmitting
  delayMicroseconds(100);
}

void cursorHome() {
  Wire.beginTransmission(slaveADDR); // transmit to device #4
  Wire.write(0xFE);  
  Wire.write(0x46);                   
  Wire.endTransmission();    // stop transmitting
  delayMicroseconds(1500);
}

void setCursor(byte pos) {
  Wire.beginTransmission(slaveADDR); // transmit to device #4
  Wire.write(0xFE);  
  Wire.write(0x45);   
  Wire.write(pos);                  
  Wire.endTransmission();    // stop transmitting
  delayMicroseconds(100);
}

void clearScreen() {
  Wire.beginTransmission(slaveADDR); // transmit to device #4
  Wire.write(0xFE);  
  Wire.write(0x51);                   
  Wire.endTransmission();    // stop transmitting
  delayMicroseconds(1500);
}
