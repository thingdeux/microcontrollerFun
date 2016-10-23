// -----------------------------------
// Remote Temp. monitor
// -----------------------------------
#include "RGBMood.h"
// PIN DEFINITIONS
#define analogSensorRight A0
#define analogMicAmp A1
#define boardLED 7
#define rgbOneRED A7
#define rgbOneGREEN A6
#define rgbOneBLUE A5
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
bool isListeningToMic = true;
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

RGBMood snoozeLight(rgbOneRED, rgbOneGREEN, rgbOneBLUE);

void setInitialBoardState() {
  pinMode(boardLED, OUTPUT);
  pinMode(rgbOneRED, OUTPUT);
  pinMode(rgbOneGREEN, OUTPUT);
  pinMode(rgbOneBLUE, OUTPUT);
  pinMode(analogSensorRight, INPUT);
  pinMode(analogMicAmp, INPUT);
  digitalWrite(boardLED, LOW);
  analogWrite(rgbOneRED, 1);
  analogWrite(rgbOneGREEN, 1);
  analogWrite(rgbOneBLUE, 0);
}

void setup() {
   Serial.begin(9600);
   setInitialBoardState();
   snoozeLight.setMode(RGBMood::RAINBOW_HUE_MODE);
   snoozeLight.setFadingSteps(200);
   snoozeLight.setFadingSpeed(25);  // 25 ms * 200 steps = 5 seconds
   snoozeLight.setHoldingTime(0);
   snoozeLight.fadeHSB(0, 255, 255); // Rainbow mode only changes hue so set saturation and brightness
   // Define particle.io exposed endpoints.
   Spark.variable("tempRight", rightTemp);
   Spark.variable("avgTemp", averageTemp);
}
void loop() {
    /*double soundLevel = captureSomeSound();    */
    rightTemp = convertVoltageToFarenheit(analogRead(analogSensorRight)) + tweakFinalOutputTemp;
    snoozeLight.tick();
    /*if (isListeningToMic == true) {
      int micInput = analogRead(analogMicAmp);
      if (micInput > 1300) {
          Serial.println(micInput);
      }
    }*/
    /*delay(5);*/
}

double convertVoltageToFarenheit(int readValue) {
    // Convert analog voltage from the Temp sensor into Farenheit
    // Particle.io output requires double so converting
    float voltage = (readValue / 4024.0) * analogVoltage;
    float tempC = (voltage - 0.5) * 100;
    float tempF = (tempC * 9.0 / 5.0) + 32.0;
    return tempF;
}

double captureSomeSound() {
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
   /*Serial.println(volts);*/
   return volts;
}

// Set party mode - this will enable the mic and the LED will move
// In sync with the noise the kids make.
void setPartyMode() {
  snoozeLight.setPartyMode(true);
}

void disablePartyMode() {
  snoozeLight.setPartyMode(false);
  snoozeLight.fadeHSB(0, 255, 50);
}
