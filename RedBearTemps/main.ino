// -----------------------------------
// Remote Temp. monitor
// -----------------------------------
#include "RGBMood.h"
// PIN DEFINITIONS
#define analogSensorRight A2
#define analogMicAmp A3
#define boardLED 7
#define rgbOneRED A7
#define rgbOneGREEN A6
#define rgbOneBLUE A5
#define rgbTwoRED A0
#define rgbTwoGREEN A1
#define rgbTwoBLUE A4
#define ledCount 2
// Definitions related to TMP 36
#define analogVoltage 3.32
#define tweakFinalOutputTemp -2.0
// RGB LED Definitions
#define bedTimeFadingSpeed 50
#define partyModeFadingSpeed 1



// GlobalVars
signed short checkTemp = 0;
double rightTemp = 0.0;
double leftTemp = 0.0;
double averageTemp = 0.0;
bool isInPartyMode = true;

// Microphone
signed int currentNoiseFloor = 1550;
int audioVoltage = 0;
signed int currentAudioLevel = 0;
/*const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;*/
RGBMood snoozeLights[2] = {
  RGBMood(rgbOneRED, rgbOneGREEN, rgbOneBLUE),
  RGBMood(rgbTwoRED, rgbTwoGREEN, rgbTwoBLUE)
};

void setInitialBoardState() {
  pinMode(boardLED, OUTPUT);
  pinMode(rgbOneRED, OUTPUT);
  pinMode(rgbOneGREEN, OUTPUT);
  pinMode(rgbOneBLUE, OUTPUT);
  pinMode(rgbTwoRED, OUTPUT);
  pinMode(rgbTwoGREEN, OUTPUT);
  pinMode(rgbTwoBLUE, OUTPUT);
  pinMode(analogSensorRight, INPUT);
  pinMode(analogMicAmp, INPUT);
  // Set all LED's to no light
  digitalWrite(boardLED, LOW);
  analogWrite(rgbOneRED, 0);
  analogWrite(rgbOneGREEN, 0);
  analogWrite(rgbOneBLUE, 0);
  analogWrite(rgbTwoRED, 0);
  analogWrite(rgbTwoGREEN, 0);
  analogWrite(rgbTwoBLUE, 0);
  // Start Rainbow Mode
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].setMode(RGBMood::RAINBOW_HUE_MODE);
    snoozeLights[i].setFadingSteps(200);
    snoozeLights[i].setFadingSpeed(25);  // 25 ms * 200 steps = 5 seconds
    snoozeLights[i].setHoldingTime(0);
    snoozeLights[i].fadeHSB(0, 255, 255); // Rainbow mode only changes hue so set saturation and brightness
  }

}

void setup() {
   Serial.begin(9600);
   setInitialBoardState();
   // Define particle.io exposed endpoints.
   Spark.variable("tempRight", rightTemp);
   Spark.variable("avgTemp", averageTemp);
   Spark.function("dim", dimLights);
   Spark.function("brighten", brightenLights);
   Spark.function("partyMode", setPartyMode);
   Spark.function("sleepy", sleepSystem);
}
void loop() {
    rightTemp = convertVoltageToFarenheit(analogRead(analogSensorRight)) + tweakFinalOutputTemp;
    for (int i = 0; i < ledCount; i++) {
      if (isInPartyMode == true) {
        processMicInput();
        snoozeLights[i].setHSB(random(0, 255), random(0, 255), currentAudioLevel);
      }
      snoozeLights[i].tick();
    }
    if (currentAudioLevel > 0) {
      currentAudioLevel -= 1.0;
    }
}

void processMicInput() {
  int micInput = analogRead(analogMicAmp);
  /*Serial.print("Microphone Input: ");
  Serial.print(micInput);
  Serial.print("  Current Audio Level: ");
  Serial.println(currentAudioLevel);*/
  if (micInput > currentNoiseFloor) {
    currentAudioLevel = (micInput - currentNoiseFloor);
  }
}

double convertVoltageToFarenheit(int readValue) {
    // Convert analog voltage from the Temp sensor into Farenheit
    // Particle.io output requires double so converting
    float voltage = (readValue / 4024.0) * analogVoltage;
    float tempC = (voltage - 0.5) * 100;
    float tempF = (tempC * 9.0 / 5.0) + 32.0;
    return tempF;
}

/*double captureSomeSound() {
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow) {
      sample = analogRead(analogMicAmp);
      if (sample < 1024) { // toss out spurious readings
         if (sample > signalMax) {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin) {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 3.3) / 1024.0;  // convert to volts
   return volts;
}*/

int dimLights(String command) {
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].fadeHSB(0, 255, 25);
  }
  return 1;
}

int brightenLights(String command) {
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].fadeHSB(0, 255, 255);
  }
  return 1;
}

// Set party mode - this will enable the mic and the LED will move
// In sync with the noise the kids make.
int setPartyMode(String command) {
  isInPartyMode = !isInPartyMode;
  if (isInPartyMode == false) {
    for (int i = 0; i < ledCount; i++) {
      snoozeLights[i].fadeHSB(0, 255, 255); // Rainbow mode only changes hue so set saturation and brightness
    }
  }
  return 1;
}

int sleepSystem(String command) {
  // Put the system to sleep for a given amount of time.
  // Cast string to int.
  long convertedInt = strtol(SLEEP_MODE_DEEP, command);
  if (convertedInt > 0) {
    System.sleep(convertedInt);
    return 1;
  }
  return 0;
}
