// -----------------------------------
// Remote Temp. monitor
// -----------------------------------

#define analogSensorRight A0
#define analogSensorLeft A1
#define analogVoltage 3.32
#define tweakFinalOutputTemp -2.0
const int boardLED = 7;
double rightTemp = 0.0;
double leftTemp = 0.0;
double averageTemp = 0.0;

void setInitialBoardState() {
  pinMode(boardLED, OUTPUT);
  pinMode(analogSensorRight, INPUT);
  digitalWrite(boardLED, LOW);
}

void setup() {
   setInitialBoardState();
   // Define particle.io exposed endpoints.
   Spark.variable("tempRight", rightTemp);
   Spark.variable("avgTemp", averageTemp);
}
void loop() {
    rightTemp = convertVoltageToFarenheit(analogRead(analogSensorRight)) + tweakFinalOutputTemp;
    delay(50);
}

double convertVoltageToFarenheit(int readValue) {
    // Convert analog voltage from the Temp sensor into Farenheit
    // Particle.io output requires double so converting
    float voltage = (readValue / 4024.0) * analogVoltage;
    float tempC = (voltage - 0.5) * 100;
    float tempF = (tempC * 9.0 / 5.0) + 32.0;
    return tempF;
}
