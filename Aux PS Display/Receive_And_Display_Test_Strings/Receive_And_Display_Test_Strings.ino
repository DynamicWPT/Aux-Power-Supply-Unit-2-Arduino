#include <Wire.h>

#define slaveADDR 0x28

const byte numChars = 64;
char receivedChars[numChars];

char stringA[21];
char stringB[21];
char stringC[21];
char stringD[21];

boolean newData = false;

unsigned long prevTime = 0;
unsigned long prevTime2 = 0;
unsigned long prevTime3 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(); 
  Wire.setClock(50000);

}

unsigned long rebootLoopTime = 0;

void loop()
{ 
  //Serial.println("OK");
  recvWithStartEndMarkers();

  while (receivedChars[0] == 'R') {
    if (millis() - rebootLoopTime >= 2000) {
      clearScreen();
      setCursor(0x00);
      writeString("Rebooting... ");
      rebootLoopTime = millis();
    }
    recvWithStartEndMarkers();
    showNewData();
  }

  //Serial.println(receivedChars[0]);
  if (millis() - prevTime >= 100) {
    if (receivedChars[0] == 'A') {
      for (int i = 0; i < 21; i++) {
        stringA[i] = receivedChars[i+1];
        if (receivedChars[i] == NULL) {
          break;
        }      
      }
    }
    if (receivedChars[0] == 'B') {
      for (int i = 0; i < 21; i++) {
        stringB[i] = receivedChars[i+1];
        if (receivedChars[i] == NULL) {
          break;
        }      
      }
    }
    if (receivedChars[0] == 'C') {
      for (int i = 0; i < 21; i++) {
        stringC[i] = receivedChars[i+1];
        if (receivedChars[i] == NULL) {
          break;
        }      
      }
    }
    if (receivedChars[0] == 'D') {
      for (int i = 0; i < 21; i++) {
        stringD[i] = receivedChars[i+1];
        if (receivedChars[i] == NULL) {
          break;
        }      
      }
    }
    prevTime = millis();
  }

  if (millis() - prevTime2 >= 1000) {
    setCursor(0x00);
    writeString(stringA);
    setCursor(0x40);
    writeString(stringB);
    setCursor(0x14);
    writeString(stringC);
    setCursor(0x54);
    writeString(stringD);
    prevTime2 = millis();
  }

  if (millis() - prevTime3 >= 30000) {
    clearScreen();
    prevTime3 = millis();
  }

  showNewData();
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
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


