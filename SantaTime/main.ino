#define relayPin D0
#define sadFaceLED A1
#define happyFaceLED A2
#define mehFaceLED A0
#define nightTimeThreshold 400
#define CommandSuccessful 1
#define CommandFailed 0

signed int relayState = LOW;

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(sadFaceLED, OUTPUT);
  pinMode(happyFaceLED, OUTPUT);
  pinMode(mehFaceLED, OUTPUT);
  digitalWrite(relayPin, LOW);
  Particle.function("santasMood", setSantasMood);
  Particle.function("relayControl", setRelayMode);
}

int setRelayMode(String command) {
  char relaySetting = command[0];
  switch (relaySetting) {
    case '0':
      if (relayState == HIGH) {
        relayState = LOW;
        digitalWrite(relayPin, LOW);
      }
    case '1':
      if (relayState == LOW) {
          relayState = HIGH;
          digitalWrite(relayPin, HIGH);
      }
  }
  return CommandFailed;
}

int setSantasMood(String command) {
  // Take the first character of the command string array
  char moodOption = command[0];
  switch (moodOption) {
    // Happy - Green
    case 'H':
      analogWrite(sadFaceLED, LOW);
      analogWrite(mehFaceLED, LOW);
      analogWrite(happyFaceLED, HIGH);
      return CommandSuccessful;
    // Sad - Red
    case 'S':
      analogWrite(happyFaceLED, LOW);
      analogWrite(mehFaceLED, LOW);
      analogWrite(sadFaceLED, HIGH);
      return CommandSuccessful;
    // Meh - Yellow
    case 'M':
      analogWrite(happyFaceLED, LOW);
      analogWrite(sadFaceLED, LOW);
      analogWrite(mehFaceLED, HIGH);
      return CommandSuccessful;
  }
  return CommandFailed;
}

void loop() {
}
