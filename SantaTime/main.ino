#define relayPin D0
#define sadFaceLED D1
#define happyFaceLED D2
#define mehFaceLED D3
#define nightTimeThreshold 400
#define CommandSuccessful 1
#define CommandFailed 0
#define flashDelayInMs = 2000;

signed int relayState = LOW;
signed int globalTimer = 0;
bool shouldFlashLights = false;

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(sadFaceLED, OUTPUT);
  pinMode(happyFaceLED, OUTPUT);
  pinMode(mehFaceLED, OUTPUT);
  // All Lights Start off
  digitalWrite(sadFaceLED, LOW);
  digitalWrite(mehFaceLED, LOW);
  digitalWrite(happyFaceLED, LOW);
  // Christmas Lights controlled through Relay should start off
  digitalWrite(relayPin, LOW);
  Particle.function("santasMood", setSantasMood);
  Particle.function("relayControl", setRelayMode);
  Serial.begin(9600);
}

int setRelayMode(String command) {
  char relaySetting = command[0];
  switch (relaySetting) {
    case '0':
        shouldFlashLights = false;
        relayState = LOW;
        digitalWrite(relayPin, LOW);
        return CommandSuccessful;
    case '1':
        globalTimer = 0;
        shouldFlashLights = true;
        return CommandSuccessful;
  }
  return CommandFailed;
}

int setSantasMood(String command) {
  // Take the first character of the command string array
  char moodOption = command[0];
  switch (moodOption) {
    // Happy - Green
    case 'H':
      digitalWrite(sadFaceLED, LOW);
      digitalWrite(mehFaceLED, LOW);
      digitalWrite(happyFaceLED, HIGH);
      return CommandSuccessful;
    // Sad - Red
    case 'S':
      digitalWrite(happyFaceLED, LOW);
      digitalWrite(mehFaceLED, LOW);
      digitalWrite(sadFaceLED, HIGH);
      return CommandSuccessful;
    // Meh - Yellow
    case 'M':
      digitalWrite(happyFaceLED, LOW);
      digitalWrite(sadFaceLED, LOW);
      digitalWrite(mehFaceLED, HIGH);
      return CommandSuccessful;
    // None
    case 'N':
      digitalWrite(happyFaceLED, LOW);
      digitalWrite(sadFaceLED, LOW);
      digitalWrite(mehFaceLED, LOW);
      return CommandSuccessful;
  }
  return CommandFailed;
}

void setRelay(int value) {
  relayState = value;
  digitalWrite(relayPin, value);
}

void processChristmasTreeLights() {
  switch (globalTimer) {
    case 1:
      setRelay(HIGH);
      break;
    case 2000:
      setRelay(LOW);
      break;
    case 4000:
      setRelay(HIGH);
      break;
    case 6000:
      setRelay(LOW);
      break;
    case 8000:
      setRelay(HIGH);
      break;
    case 10000:
      setRelay(LOW);
      break;
    case 12000:
      setRelay(HIGH);
      break;
    case 14000:
      setRelay(LOW);
      globalTimer = 0;
      shouldFlashLights = false;
      break;
  }
}

void loop() {
  Serial.println(globalTimer);
  if (shouldFlashLights == true) {
    globalTimer += 1;
    processChristmasTreeLights();
  }
}
