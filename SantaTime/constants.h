#define relayPin D4
#define piezoPIN D3
#define rgbRedPin D0
#define rgbGreenPin D1
#define rgbBluePin D2
#define CommandSuccessful 1
#define CommandFailed 0

// Note: BPM to MS conversation = 60000 / MS
#define Tempo 342



// Note: Timing Chart in milliseconds at 175 Beats per minute (Jingle Bells' BPM)
// Quarter - 341
// Dotted Quarter - 512
// Whole 1364
// Dotted Half 1023
// Half 682
// Eigth 171
// Sixteenth 85

const signed short int QuarterNote = 341;
const signed short int DottedQuarterNote = 512;
const signed short int WholeNote = 1364;
const signed short int DottedHalfNote = 1023;
const signed short int HalfNote = 682;
const signed short int EigthOfANote = 171;
const signed short int SixteenthOfANote = 171;

// Notes in the melody - the length here needs to match with the durations array
// Below.  Not ideal but....cheap
const int jingleBells[] = {
  // Jingle Bells, Jingle Bells
  NOTE_E5, SILENCE, NOTE_E5, SILENCE, NOTE_E5, SILENCE, NOTE_E5, SILENCE, NOTE_E5, SILENCE, NOTE_E5,
  // Jingle all the way
  SILENCE, NOTE_E5, SILENCE, NOTE_G5, SILENCE, NOTE_C5, SILENCE, NOTE_D5, SILENCE, NOTE_E5
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
const int noteDurations[] = {
      /*   E3        S        E3          S         E3           S          E3          S         E3          S       E3 */
      WholeNote, Tempo,  WholeNote, Tempo,  WholeNote, Tempo + 2,  WholeNote, Tempo,  WholeNote, Tempo,  WholeNote,
      /* S        E3         S        G3         S        C3         S        D3         S        E3 */
      Tempo,  WholeNote, Tempo, WholeNote, Tempo, WholeNote, Tempo, WholeNote, Tempo, WholeNote
};
