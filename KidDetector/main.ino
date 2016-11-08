#define motionSensorPin D6
#define transistorPin D2
#define photoTransistorPin A2
#define onboardLED D7
#define nightTimeThreshold 400
#define delayBeforeSendingAnotherNotificationInMillis 4000
#define delayBeforeResettingPIR 30000
#define pirWarmupTimeInMillis 1200000 // Give the pir 2 minutes to warm up.
signed int motionSensorState = LOW;
int lastPublishedMotionSeconds = 0;
int lightLevel = 0;
bool isMotionSensorEnabled = false;
bool isCheckingForNightTime = false;
bool isCheckingForDayTime = false;
int lastCheckedForNightTime = 0;
int lastCheckedForDayTime = 0;
bool isPIRWarmingUp = false;
int pirWarmUpTime = 0;

void setup() {
  pinMode(motionSensorPin, INPUT);
  pinMode(transistorPin, OUTPUT);
  pinMode(photoTransistorPin, INPUT);
  pinMode(onboardLED, OUTPUT);
  digitalWrite(onboardLED, LOW);
  // Turn the motion sensor on
  digitalWrite(transistorPin, motionSensorState);
  Particle.variable("light", lightLevel);
}

void loop() {
  lightLevel = analogRead(photoTransistorPin);
  if (lightLevel <= nightTimeThreshold) {
    checkForNightTime();
  } else {
    checkForDaytime();
  }
  if (motionSensorState == HIGH && isPIRWarmingUp == false) {
    checkForMotion();
  } else if (motionSensorState == HIGH && isPIRWarmingUp == true) {
    if (millis() - pirWarmUpTime > delayBeforeResettingPIR) {
      isPIRWarmingUp = false;
    }
  }
}

void checkForNightTime() {
  if (isMotionSensorEnabled == false && isCheckingForNightTime == false) {
    // Make sure that there wasn't some light anomoly
    // And indeed the lights are fully out.
    isCheckingForNightTime = true;
    lastCheckedForNightTime = millis();
  }

  // We're currently checking for nighttime and it's been at least 10 seconds
  // Enable nighttime mode.
  if (isCheckingForNightTime == true && (millis() - lastCheckedForNightTime) >= delayBeforeResettingPIR) {
    isCheckingForDayTime = false;
    isCheckingForNightTime = false;
    isMotionSensorEnabled = true;
    motionSensorState = HIGH;
    isPIRWarmingUp = true;
    pirWarmUpTime = millis();
    digitalWrite(onboardLED, HIGH);
    digitalWrite(transistorPin, motionSensorState);
  }
}

void checkForDaytime() {
  if (isMotionSensorEnabled == true && isCheckingForDayTime == false) {
    // Make sure that there wasn't some light anomoly
    // And indeed the lights are fully out.
    isCheckingForDayTime = true;
    lastCheckedForDayTime = millis();
  }
  // We're currently checking for nighttime and it's been at least 10 seconds
  // Enable nighttime mode.
  if (isCheckingForDayTime == true && (millis() - lastCheckedForDayTime) >= delayBeforeResettingPIR) {
    isCheckingForNightTime = false;
    isCheckingForDayTime = false;
    isMotionSensorEnabled = false;
    motionSensorState = LOW;
    digitalWrite(onboardLED, motionSensorState);
    digitalWrite(transistorPin, motionSensorState);
  }
}

void checkForMotion() {
    int movementDetected = digitalRead(motionSensorPin);
    if (movementDetected) {
        notifyMotionDetected();
    }
}

void notifyMotionDetected() {
  int currentMillis = millis();
  if (currentMillis - lastPublishedMotionSeconds >
      delayBeforeSendingAnotherNotificationInMillis) {
          Particle.publish("MotionDetected", String(currentMillis), PRIVATE);
  }
  lastPublishedMotionSeconds = currentMillis;
}
