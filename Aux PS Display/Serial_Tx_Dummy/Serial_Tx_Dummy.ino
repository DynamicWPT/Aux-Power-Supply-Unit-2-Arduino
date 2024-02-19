float Vout = 0.0;
int Aout = 0;
int Vin = 0;
unsigned long prevTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  if (millis() - prevTime >= 1000) {
    char serialDataBuffer[64] = "\0";

    Vout++;
    Aout++;
    Vin+=10;
    if (Vout > 72) {
      Vout = 0;
    }
    if (Aout > 120) {
      Aout = 0;
    }
    if (Vin > 750) {
      Vin = 0;
    }

    strcpy(serialDataBuffer, String(Vout, 1).c_str());
    strcat(serialDataBuffer, ",");
    strcat(serialDataBuffer, String(Aout).c_str());
    strcat(serialDataBuffer, ",");
    strcat(serialDataBuffer, String(Vin).c_str());
    strcat(serialDataBuffer, ",");
    strcat(serialDataBuffer, "\n");
    Serial.print(serialDataBuffer);

    prevTime = millis();
  }

}
