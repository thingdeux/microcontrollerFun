// -----------------------------------
// Remote Temp. monitor
// -----------------------------------
#include "RGBMood.h"
// Pins
#define analogSensorRight A2
#define analogMicAmp A3
#define boardLED 7
#define rgbOneRED A7
#define rgbOneGREEN A6
#define rgbOneBLUE A5
#define rgbTwoRED A0
#define rgbTwoGREEN A1
#define rgbTwoBLUE A4
// Temp Sensor
#define analogVoltage 3.32
#define tweakFinalOutputTemp -2.0
// RGB LED
#define bedTimeFadingSpeed 50
#define partyModeFadingSpeed 1
#define ledCount 2  // Note: Update this should another set of LEDs be added.
// Microphone
#define minNoiseFloor 1450
#define scanTimeForNoiseFloorInMilli 3000
#define maxTimeAllowedForPartyModeInMillis 900000  // 15 Mins

// GlobalVars
double rightTemp = 0.0;
double leftTemp = 0.0;
bool isInPartyMode = false;
/*signed int maxTimeAllowedForPartyMode = maxTimeAllowedForPartyModeInMillis*/
signed int maxTimeAllowedForPartyMode = 60000;
int partyModeStartTime = 0;

// Microphone
int currentNoiseFloor = 1560;
signed short audioVoltage = 0;
signed short currentAudioLevel = 0;

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
   Spark.variable("noiseFloor", currentNoiseFloor);
   Spark.function("dim", dimLights);
   Spark.function("brighten", brightenLights);
   Spark.function("partyMode", setPartyMode);
   Spark.function("sleepy", sleepSystem);
}
void loop() {
    rightTemp = convertVoltageToFarenheit(analogRead(analogSensorRight)) + tweakFinalOutputTemp;
    // RGB Per-Loop Ticks and logic
    for (short i = 0; i < ledCount; i++) {
      if (isInPartyMode == true) {
        processMicInput();
        snoozeLights[i].setHSB(random(0, 255), random(0, 255), currentAudioLevel);
        if (millis() - partyModeStartTime > maxTimeAllowedForPartyMode) {
          setPartyMode("OFF");
        }
      }
      snoozeLights[i].tick();
    }
    if (currentAudioLevel > 0) {
      currentAudioLevel -= 1.0;
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
/* ------------- MICROPHONE ---------- */
void processMicInput() {
  short micInput = analogRead(analogMicAmp);
  if (micInput > currentNoiseFloor) {
    currentAudioLevel = (micInput - currentNoiseFloor);
  }
}

// Blocking - Measure the current room noise level.
int acquireNoiseFloor() {
  // Dim all LEDS
  setLEDBrightness(5);
  // Take a second or so to acquire the current room audio level.
  float startingMillis = millis();
  while (millis() - startingMillis < scanTimeForNoiseFloorInMilli) {
    short micInput = analogRead(analogMicAmp);
    // Throw away anything that's not at least over 1450
    // The microphones idle state is 1450
    if (micInput > minNoiseFloor &&
        micInput > currentNoiseFloor ||
        // It's possible a large reading could have thrown off the measurement,
        // if the floor is less than the current floor by a high number then allow resetting.
        micInput < (currentNoiseFloor - 500)) {
        currentNoiseFloor = micInput;
    }
    delay(5);
  }
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].fadeHSB(0, 255, 255); // Rainbow mode only changes hue so set saturation and brightness
  }
}

/* ------ LED ------ */
void setLEDBrightness(int brightness) {
  for (int i = 0; i < ledCount; i++) {
      analogWrite(rgbOneRED, brightness);
      analogWrite(rgbOneGREEN, brightness);
      analogWrite(rgbOneBLUE, brightness);
      analogWrite(rgbTwoRED, brightness);
      analogWrite(rgbTwoGREEN, brightness);
      analogWrite(rgbTwoBLUE, brightness);
  }
}

/* ------ Exposed API Functions ------- */
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
  } else {
    partyModeStartTime = millis();
    acquireNoiseFloor();
  }
  return 1;
}

int sleepSystem(String command) {
  // Put the system to sleep for a given amount of time.
  // Cast string to int.
  /*long convertedInt = strtol(SLEEP_MODE_DEEP, command);*/
  /*if (convertedInt > 0) {
    System.sleep(convertedInt);
    return 1;
  }*/
  return 0;
}
