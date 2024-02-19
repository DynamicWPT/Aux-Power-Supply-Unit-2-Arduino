char* stringA = "<AV:72.0 A:120 Vin:750>\n";
char* stringB = "<BTemp:100.0C>\n";
char* stringC = "<CMode: >\n";
char* stringD = "<DFault: >\n";
unsigned long prevTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {

    Serial.write(stringA);
    delay(250);
    Serial.write(stringB);
    delay(250);
    Serial.write(stringC);
    delay(250);
    Serial.write(stringD);
    delay(250);

}
