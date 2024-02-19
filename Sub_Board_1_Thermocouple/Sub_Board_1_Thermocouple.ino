#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>

//Thermocouple
#define spiDeviceNum 8
int spiPinAssign[] = {4, 5, 6, 7, A2, A3, 2, 3}; //SPI pins
float thermTemp[spiDeviceNum] = {0, 0, 0, 0, 0, 0, 0, 0}; //Temp arrays

void setupTimer1();
void setPWM1A(float f);
void setPWM1B(float f);
float PWM_Ratio0 = 0.2;
float PWM_Ratio1 = 0;

RTC_DS1307 rtc;

char data[64] = " ";

void setup() {
  // put your setup code here, to run once:
  delay(45000);

  Serial.begin(9600);

  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.begin();

  // Set CS of SPI devices based on spiPinAssign array
  for (int i = 0; i < spiDeviceNum; i++){
    pinMode(spiPinAssign[i], OUTPUT);
    digitalWrite(spiPinAssign[i], HIGH);
  }

  pinMode(9,OUTPUT); //1A
  pinMode(10,OUTPUT); //1B
  setupTimer1();

  setPWM1A(PWM_Ratio0); //set duty % on pin 9
  setPWM1B(PWM_Ratio1); //set duty % on pin 10

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  //rtc.adjust(DateTime(2024, 2, 16, 12, 48, 0));
  DateTime now = rtc.now();
  //Serial.println(now.year());

  strcpy(data, String(now.year()).c_str());
  strcat(data, "\r");
  strcat(data, String(now.month()).c_str());
  strcat(data, "\r");
  strcat(data, String(now.day()).c_str());
  strcat(data, "\r");
  strcat(data, String(now.hour()).c_str());
  strcat(data, "\r");
  strcat(data, String(now.minute()).c_str());
  strcat(data, "\r");
  strcat(data, String(now.second()).c_str());
  strcat(data, "\n");
  Serial.write(data);

  delay(5000);
}

unsigned long prevTime = 0;
long index = 33;
int controlTemp = thermTemp[4];
String LVR = "F";
String OTemp = "F";

void loop() {
  //LVR
  if (digitalRead(8) == HIGH) {
    LVR = "F";
  } else {
    LVR = "T";
  }

  //OTemp
  if (999 > controlTemp > 150) {
    digitalWrite(A1, HIGH);
    OTemp = "T";
  } else {
    digitalWrite(A1, LOW);
    OTemp = "F";
  }

  //Temperature Analog Output
  PWM_Ratio1 = 1 - ((100/120) * controlTemp + 16) / 100;
  if (PWM_Ratio1 > 1) {
    setPWM1B(1);
  } else if (PWM_Ratio1 < 0) {
    setPWM1B(0);
  } else {
    setPWM1B(PWM_Ratio1);
  }

  if (millis() - prevTime >= 950) {
    //Index
    strcpy(data, String(char(index)).c_str());
    strcat(data, "\r");
    index++;

    if (index > 255) {
      index = 33;
    }

    strcat(data, "99999\r99999\r");
    for (int i = 0; i < spiDeviceNum; i++) {
      thermTemp[i] = getTemp(spiPinAssign[i]);

      if (thermTemp[i] >= 2047) {
        thermTemp[i] = 999;
      }
      // Serial.println(thermTemp);
      strcat(data, String(thermTemp[i] * 10, 0).c_str());
      strcat(data, "\r");  // concatonate CR
    }
    receiveData();

    strcat(data, String(PWM_Ratio0 * 100).c_str());
    strcat(data, "\r");  

    //Low Voltage Relay State Boolean
    strcat(data, LVR.c_str());
    strcat(data, "\r");  

    //OTemp State Boolean
    strcat(data, OTemp.c_str());
    strcat(data, "\n"); 

    Serial.write(data); 
    prevTime = millis();
  }
}

void receiveData() {
  int address = 8;
  int bytesToRead = 12;
  int i = 0;

  Wire.requestFrom(address, bytesToRead);
  while (Wire.available()) {
    data[i + 2] = Wire.read();
    i++;
  }
}

void setupTimer1() {
    //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << CS10) | (1 << WGM13);
    ICR1 = 320;
    OCR1A = 0;
    OCR1B = 0;
}

//equivalent of analogWrite on pin 9
void setPWM1A(float f) {
    f=f<0?0:f>1?1:f;
    OCR1A = (uint16_t)(320*f);
}

//equivalent of analogWrite on pin 10
void setPWM1B(float f) {
    f=f<0?0:f>1?1:f;
    OCR1B = (uint16_t)(320*f);
}

// Get SPI data from MAX31855 chip serial output
uint32_t spiread32(void) {
  uint32_t v;
  v = SPI.transfer(0);
  v <<= 8;
  v |= SPI.transfer(0);
  v <<= 8;
  v |= SPI.transfer(0);
  v <<= 8;
  v |= SPI.transfer(0);
  return v;
}

// Get Thermocouple Temp in Celcius
float getTemp(int pin) {
  digitalWrite(pin, LOW); 
  uint32_t data = spiread32();
  if (data & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    data = 0xFFFFC000 | ((data >> 18) & 0x00003FFF);
  } else {
    // Positive value, just drop the lower 18 bits.
    data >>= 18;
  }
  digitalWrite(pin, HIGH);
  // Format Data to Celcius
  float centigrade = data * 0.25;
  return centigrade;
}