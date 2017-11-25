#include "pitches.h"
#include "constants.h"

// Note: Cannot read state directly from the transistor - hence unknown.
signed int relayState = OFF;
signed int lightTimer = 0;
signed int musicTimer = 0;
bool shouldFlashLights = false;
bool shouldPlayMusic = false;

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(rgbGreenPin, OUTPUT);
  pinMode(rgbBluePin, OUTPUT);
  pinMode(rgbRedPin, OUTPUT);
  // All Lights Start off
  analogWrite(rgbRedPin, 0);
  analogWrite(rgbBluePin, 0);
  analogWrite(rgbGreenPin, 0);
  // Christmas Lights controlled through Relay should start off
  digitalWrite(relayPin, LOW);

  Particle.variable("rState", relayState);
  Particle.function("santasMood", setSantasMood);
  Particle.function("relayControl", setRelayMode);
  Particle.function("syncRelay", syncRelay);

  /*Serial.begin();*/
}

int syncRelay(String command) {
    // If the relayState ever gets into an unknown state - we can force
    // A resume here and shut the thing off.
    setRelay(LOW);
    relayState = OFF;
    return CommandSuccessful;
}

int setRelayMode(String command) {
  int relaySetting = int(command.toInt());
  switch (relaySetting) {
    case OFF:
        Serial.println("RELAY OFF");
        shouldFlashLights = false;
        relayState = OFF;
        setRelay(LOW);
        return CommandSuccessful;
    case ON:
      Serial.println("RELAY ON");
        lightTimer = 0;
        shouldFlashLights = false;
        relayState = ON;
        setRelay(HIGH);
        return CommandSuccessful;
    case LAUGH:
      if (relayState != LAUGH) {
        Serial.println("RELAY LAUGH");
        lightTimer = 0;
        shouldFlashLights = true;
        relayState = LAUGH;
      }
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
      analogWrite(rgbBluePin, 0);
      analogWrite(rgbRedPin, 0);
      analogWrite(rgbGreenPin, 255);
      if (!shouldPlayMusic) { playJingleBells(); }
      return CommandSuccessful;
    // Sad - Blue
    case 'S':
      analogWrite(rgbRedPin, 0);
      analogWrite(rgbGreenPin, 0);
      analogWrite(rgbBluePin, 255);
      return CommandSuccessful;
    // Neutral - Yellow
    case 'M':
      analogWrite(rgbRedPin, 252);
      analogWrite(rgbGreenPin, 239);
      analogWrite(rgbBluePin, 0);
      return CommandSuccessful;
    // None
    case 'N':
      analogWrite(rgbRedPin, 0);
      analogWrite(rgbGreenPin, 0);
      analogWrite(rgbBluePin, 0);
      return CommandSuccessful;
  }
  return CommandFailed;
}

void setRelay(int value) {
  digitalWrite(relayPin, value);
}

// Need to make sure that the animation doesn't block - hence, no delays
// Process the animation on a timer and allow for transport communication
// To continue controlling other elements.
void processChristmasTreeLights() {
  switch (lightTimer) {
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
      relayState = OFF;
      lightTimer = 0;
      shouldFlashLights = false;
      break;
  }
}


// TODO: Encapsulate this logic in a class.

signed short int currentNoteIndex = 0;
signed short int currentNote = SILENCE;
signed short int currentNoteDuration = Tempo;
signed short int jingleBellArraySize = (sizeof(jingleBells) / sizeof(jingleBells[0]));

void playJingleBells() {
  currentNoteIndex = 0;
  setCurrentNote();
  shouldPlayMusic = true;
}

void setCurrentNote() {
  signed short int pitch = jingleBells[currentNoteIndex];
  if (pitch == SILENCE) {
    noTone(piezoPIN);
  } else {
    currentNoteDuration = noteDurations[currentNoteIndex];
    tone(piezoPIN, pitch, currentNoteDuration);
  }
}

void processNotes() {
  if (musicTimer >= currentNoteDuration) {
      currentNoteIndex += 1;
      if (currentNoteIndex < jingleBellArraySize) {
          setCurrentNote();
      } else {
        currentNoteIndex = 0;
        shouldPlayMusic = false;
        noTone(piezoPIN);
      }
      musicTimer = 0;
  }
}

void loop() {
  if (shouldFlashLights == true) {
    lightTimer += 1;
    processChristmasTreeLights();
  }

  if (shouldPlayMusic) {
    /*Serial.println(musicTimer);*/
    musicTimer += 1;
    processNotes();
  }
}
