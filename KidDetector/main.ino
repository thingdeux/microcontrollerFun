#define motionSensorPin D6
#define transistorPin D0
#define ledPin 8
#define delayBeforeSendingAnotherNotificationInMillis 4000
int lastPublishedMotionSeconds = 0;

typedef enum motionSensorPowerState {
  Off,
  On
};

int setMotionCensorState(motionSensorPowerState state) {
  if (state == On) {
    digitalWrite(transistorPin, HIGH);
  } else {
    digitalWrite(transistorPin, LOW);
  }
  return 0;
}

void setup() {
  pinMode(motionSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(transistorPin, OUTPUT);
  // Turn the motion sensor on
  setMotionCensorState(Off);
}

void loop() {
  int movementDetected = digitalRead(motionSensorPin);
  if (movementDetected) {
    publishMotion();
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void publishMotion() {
  int currentMillis = millis();
  if (currentMillis - lastPublishedMotionSeconds >
      delayBeforeSendingAnotherNotificationInMillis) {
        digitalWrite(ledPin, HIGH);
          Particle.publish("MotionDetected", String(currentMillis), PRIVATE);
  }
  lastPublishedMotionSeconds = currentMillis;
}
