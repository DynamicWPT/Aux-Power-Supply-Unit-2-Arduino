#include <Wire.h>
#define PIN_SENSE1 2 //where we connected the fan sense pin. Must be an interrupt capable pin (2 or 3 on Arduino Uno)
#define PIN_SENSE2 3
#define DEBOUNCE 10000 //0 is fine for most fans, crappy fans may require 10 or 20 to filter out noise
#define FANSTUCK_THRESHOLD 500000 //if no interrupts were received for 500ms, consider the fan as stuck and report 0 RPM

unsigned long volatile ts1=0, ts2=0, ts3=0, ts4=0;

void setup() {
  Serial.begin(9600);
  Wire.begin(8); // join the I2C bus with address 8
  Wire.onRequest(requestEvent); // call requestEvent when data is requested

  pinMode(PIN_SENSE1,INPUT_PULLUP); //set the sense pin as input with pullup resistor
  pinMode(PIN_SENSE2,INPUT_PULLUP); //set the sense pin as input with pullup resistor
  attachInterrupt(digitalPinToInterrupt(PIN_SENSE1),tachISR1,FALLING); //set tachISR to be triggered when the signal on the sense pin goes low
  attachInterrupt(digitalPinToInterrupt(PIN_SENSE2),tachISR2,FALLING); //set tachISR to be triggered when the signal on the sense pin goes low]
  Serial.println("ok");
 
}

char packet1[13] = " ";
unsigned long prevTime = 0;

int fanRPM1 = 0;
int fanRPM2 = 0;
char fanRPM1string[6] = " ";
char fanRPM2string[6] = " ";

void loop() {
  fanRPM1 = calcRPM(ts1, ts2);
  fanRPM2 = calcRPM(ts3, ts4);

  if (fanRPM1 > 9999) {
    strcpy(fanRPM1string, String(fanRPM1).c_str());
  }
  else if (fanRPM1 > 999 & fanRPM1 <= 9999) {
    strcpy(fanRPM1string, "0");
    strcat(fanRPM1string, String(fanRPM1).c_str());
  }
  else if (fanRPM1 > 99 & fanRPM1 <= 999) {
    strcpy(fanRPM1string, "00");
    strcat(fanRPM1string, String(fanRPM1).c_str());
  }
  else if (fanRPM1 > 9 & fanRPM1 <= 99) {
    strcpy(fanRPM1string, "000");
    strcat(fanRPM1string, String(fanRPM1).c_str());
  }
  else if (fanRPM1 > 0 & fanRPM1 <= 9) {
    strcpy(fanRPM1string, "0000");
    strcat(fanRPM1string, String(fanRPM1).c_str());
  }
  else {
    strcpy(fanRPM1string, "00000");
  } 

  if (fanRPM2 > 9999) {
    strcpy(fanRPM2string, String(fanRPM2).c_str());
  }
  else if (fanRPM2 > 999 & fanRPM2 <= 9999) {
    strcpy(fanRPM2string, "0");
    strcat(fanRPM2string, String(fanRPM2).c_str());
  }
  else if (fanRPM2 > 99 & fanRPM2 <= 999) {
    strcpy(fanRPM2string, "00");
    strcat(fanRPM2string, String(fanRPM2).c_str());
  }
  else if (fanRPM2 > 9 & fanRPM2 <= 99) {
    strcpy(fanRPM2string, "000");
    strcat(fanRPM2string, String(fanRPM2).c_str());
  }
  else if (fanRPM2 > 0 & fanRPM2 <= 9) {
    strcpy(fanRPM2string, "0000");
    strcat(fanRPM2string, String(fanRPM2).c_str());
  }
  else {
    strcpy(fanRPM2string, "00000");
  }

  if (millis() - prevTime >= 500) {
    for (int i = 0; i < 5; i++) {
      packet1[i] = fanRPM1string[i];
      packet1[i+6] = fanRPM2string[i];
    }
    packet1[5] = '\r';
    packet1[11] = '\r';

    prevTime = millis();
  }
}
void requestEvent(int bytes) {
  Wire.write(packet1); // respond with message of 10 bytes as expected by master
}

//Interrupt handler. Stores the timestamps of the last 2 interrupts and handles debouncing
void tachISR1() {
    unsigned long m=micros();
    if((m-ts2)>DEBOUNCE){
        ts1=ts2;
        ts2=m;
    }
}

void tachISR2() {
    unsigned long m=micros();
    if((m-ts4)>DEBOUNCE){
        ts3=ts4;
        ts4=m;
    }
}

//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.
unsigned long calcRPM(unsigned long volatile tsA, unsigned long volatile tsB){
    if(micros()-tsB<FANSTUCK_THRESHOLD&&tsB!=0){
        return (60000000.0/(tsB-tsA))/2.0;
    }else return 0;
}