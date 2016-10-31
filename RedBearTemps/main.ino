// -----------------------------------
// -      Remote Temp. monitor       -
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
#define bedTimeLightDim 84
#define bedTimeFadingSpeed 50
#define ledCount 2  // Note: Update this should another set of LEDs be added.
// Microphone
#define minNoiseFloor 1450
#define scanTimeForNoiseFloorInMilli 3000
#define maxTimeAllowedForPartyModeInMillis 900000  // 15 Mins

// GlobalVars
double rightTemp = 0.0;
double leftTemp = 0.0;
signed int RGBBrightness = 255;
bool isInPartyMode = false;
signed int maxTimeAllowedForPartyMode = maxTimeAllowedForPartyModeInMillis;
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
  setRainbowMode();
}

void setup() {
   setInitialBoardState();
   // Define particle.io exposed endpoints.

   // API Exposed Variables
   Particle.variable("tempRight", rightTemp);
   /*Particle.variable("noiseFloor", currentNoiseFloor);*/
   Particle.variable("brightness", RGBBrightness);
   // API Functions
   Particle.function("partyMode", setPartyMode);
   Particle.function("lightLevel", setBrightness);
   Particle.function("fixedColor", setColorInHSB);
   Particle.function("rainbowMode", enableRainbowMode);
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
  currentNoiseFloor = minNoiseFloor;
  forceLEDColorBrightness(2);
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
        micInput < (currentNoiseFloor - 300)) {
        currentNoiseFloor = micInput;
    }
    delay(5);
  }
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].fadeHSB(0, 255, RGBBrightness); // Rainbow mode only changes hue so set saturation and brightness
  }
}

/* ------ LED ------ */
/*
  This method doesn't use RGBMoods library - this forcefully sets
  the LED pins using analogWrite immediately.  Should be used only
  for immediately dimming the leds or setting them all to max white.
*/
void forceLEDColorBrightness(int brightness) {
  for (int i = 0; i < ledCount; i++) {
      analogWrite(rgbOneRED, brightness);
      analogWrite(rgbOneGREEN, brightness);
      analogWrite(rgbOneBLUE, brightness);
      analogWrite(rgbTwoRED, brightness);
      analogWrite(rgbTwoGREEN, brightness);
      analogWrite(rgbTwoBLUE, brightness);
  }
}

// Set RGBMood brightness
void setRGBBrightness(int level) {
  RGBBrightness = level;
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].fadeHSB(0, 255, RGBBrightness);
  }
}

// Set a fixed color on all LEDS
void setLEDFixedColor(int hue, int saturation, int brightness) {
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].setMode(RGBMood::FIX_MODE);
    snoozeLights[i].setHSB(hue, saturation, brightness);
  }
}

void setRainbowMode() {
  for (int i = 0; i < ledCount; i++) {
    snoozeLights[i].setMode(RGBMood::RAINBOW_HUE_MODE);
    snoozeLights[i].setFadingSteps(200);
    snoozeLights[i].setFadingSpeed(bedTimeFadingSpeed);  // 25 ms * 200 steps = 5 seconds
    snoozeLights[i].setHoldingTime(3);
    snoozeLights[i].fadeHSB(0, 255, 255); // Rainbow mode only changes hue so set saturation and brightness
  }
}

/* ------ Exposed API Functions ------- */
int setColorInHSB(String command) {
  /*
   * Take a comma deliminated HSB value and set RGB pins to it.
   * Usage - "args=245,245,234"
   */
    isInPartyMode = false;
    // Create a new char array that's the size of all commands + 1
    char * params = new char[command.length() + 1];
    strcpy(params, command.c_str());  // Make the command string mutable by copy

    // Kind of an odd syntax here but the first call pops the left most token
    // Before the delimiter and stores a pointer to it in hue, 2nd pops the
    // Next and stores in saturation and so on.
    char * hue = strtok(params, ",");
    char * saturation = strtok(NULL, ",");
    char * brightness = strtok(NULL, ",");

    if(hue != NULL && saturation != NULL && brightness != NULL) {
      int h = atoi(hue);
      int s = atoi(saturation);
      int b = atoi(brightness);
      setLEDFixedColor(h, s, b);
      return 1;
    }
    return -1;
}

int setBrightness(String command) {
  // Take the first character of the command string array
  char lightCommand = command[0];
  switch (lightCommand) {
    case '0':
      setRGBBrightness(0);
      return 1;
    case '1':
      setRGBBrightness(43);
      return 1;
    case '2':
      setRGBBrightness(86);
      return 1;
    case '3':
      setRGBBrightness(129);
      return 1;
    case '4':
      setRGBBrightness(172);
      return 1;
    case '5':
      setRGBBrightness(215);
      return 1;
    case '6':
      setRGBBrightness(255);
      return 1;
  }
  return 0;
}

// Set party mode - this will enable the mic and the LED will move
// In sync with the noise the kids make.
int setPartyMode(String command) {
  isInPartyMode = !isInPartyMode;
  if (isInPartyMode == false) {
    setRainbowMode();
    setRGBBrightness(129);
  } else {
    acquireNoiseFloor();
    partyModeStartTime = millis();
  }
  return 1;
}

int enableRainbowMode(String command) {
  isInPartyMode = false;
  setRainbowMode();
  setRGBBrightness(255);
  return 1;
}
// TODO: Add sleep with interrupt support.
