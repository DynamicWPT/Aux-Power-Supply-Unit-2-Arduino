#include <Wire.h>

#define slaveADDR 0x28

const byte numChars = 64;
char receivedChars[numChars];

boolean newData = false;
unsigned long prevTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(); 
  Wire.setClock(50000);

}

char * Vout;
char * Aout;
char * Vin;

String data[10] = {"","",""};

char * pch;

void loop()
{ 
    recvWithEndMarker();
    showNewData();
  
  if (millis() - prevTime >= 2000) {
    char VoutDisp[7] = "V:";
    char AoutDisp[6] = "A:";
    char VinDisp[7] = "Vin:";

    int i = 0;
    pch = strtok(receivedChars, ",");
    data[i] = pch;

    while (pch != NULL) {
      i++;
      pch = strtok(NULL, ",");
      data[i] = pch;
    }

    strcat(VoutDisp, String(data[0]).c_str());
    setCursor(0x00);
    writeString(VoutDisp);
    strcat(AoutDisp, String(data[1]).c_str());
    setCursor(0x07);
    writeString(AoutDisp);
    strcat(VinDisp, String(data[2]).c_str());
    setCursor(0x0D);
    writeString(VinDisp);
    prevTime = millis();
  }
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}

void writeString(char* data) {
  Wire.beginTransmission(slaveADDR); // transmit to device #4
  Wire.write(data);
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



