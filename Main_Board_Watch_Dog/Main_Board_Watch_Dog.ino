#include <SoftwareSerial.h>
#include <Wire.h>

const byte numChars = 64;
char receivedChars[numChars];
boolean newData = false;
boolean rebootStatus = false;
boolean serialRecvTimeout = false;

// Set up a new SoftwareSerial object
SoftwareSerial mySerial (A4, A5);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);
  
  reboot();
}

int volt1 = 0;
int volt2 = 0;
int volt3 = 0;

unsigned long prevTime = 0;
char sendToMyRio[64] = " ";
long index = 33;
String reset24V = "F";


void loop() {
  if (digitalRead(5) == HIGH) {
    reset24V = "T";
  } else {
    reset24V = "F";
  }

  volt1 = analogRead(A6);
  volt2 = analogRead(A7);
  volt3 = analogRead(A0);

  recvWithStartEndMarkers();
  transmitNewData();
  
  if (millis() - prevTime >= 1000) {
    //Index
    strcpy(sendToMyRio, String(char(index)).c_str());
    strcat(sendToMyRio, "\r");
    index++;

    if (index > 255) {
      index = 33;
    }

    strcat(sendToMyRio, String(volt1).c_str());
    strcat(sendToMyRio, "\r");
    strcat(sendToMyRio, String(volt2).c_str());
    strcat(sendToMyRio, "\r");
    strcat(sendToMyRio, String(volt3).c_str());
    strcat(sendToMyRio, "\r");

    strcat(sendToMyRio, reset24V.c_str());
    strcat(sendToMyRio, "\n");

    Serial.write(sendToMyRio);

    if (reset24V == "T") {
      delay(4000);
      digitalWrite(4, LOW);
      delay(2000);
      reboot();
    }
    
    prevTime = millis();
  }
}

void reboot() {
  while (Serial.available() > 0) {
    Serial.read();
  }
  digitalWrite(4, HIGH);
  while (Serial.available() == 0) {
    rebootStatus = true;
    mySerial.write("<Rebooting... >");
    delay(500);
  }
  rebootStatus = false;
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
    unsigned long timeoutTimer;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true && millis() - timeoutTimer >= 200) {
            char receivedChars[numChars];
            serialRecvTimeout = true;
            break;
        }

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = rc; 
                receivedChars[ndx + 1] = '\0';// terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            timeoutTimer = millis();
            recvInProgress = true;
            receivedChars[ndx] = rc;
            ndx++;
        }
    }
}

void transmitNewData() {
    if (newData == true) {
        //Serial.print("This just in ... ");
        mySerial.write(receivedChars);
        newData = false;
    }
}