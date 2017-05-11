#include <DigiMIDI.h>

//Implements a 10 step volume control and 3 keys.
//Volume control attached to P5 (Reset disabled)
//Keys implement in P0,P1 and P2 (P3 and P4 cannot be used as they are involved in USB D- and D+ pins)

//For Digital Input/Output 0 is P0, 1 is P1, 2 is P3, etc.
//For Analog Input 0 is P5, 1 is P2, 2 is P4, 3 is P3. (Only 4 pins support Analog Input)
//For PWM Output, 0 is P0, 1 is P1, 4 is P4 (Only 3 Pins support PWM) 


DigiMIDIDevice midi;

#define NumDigital 3
#define NumAnalog  1
#define debounceTime 10 //10 milliseconds

int dPins[NumDigital] = {PB0,PB1,PB2};
int aPins[NumAnalog] = {0};
int dNotes[NumDigital] = {0x3C,0x3D,0x3E};
int ldPins[NumDigital] = {-1,-1,-1};
int sdPins[NumDigital] = {0,0,0};
unsigned long tdPins[NumDigital];
int laPins[NumAnalog] = {-1};
int aNotes[NumAnalog] = {0x10};
int dVal[NumDigital];
int aVal[NumAnalog];
int i;

void setup() {
  // put your setup code here, to run once:
  for(i=0;i<NumDigital;i++) {
    pinMode(dPins[i],INPUT_PULLUP);
    digitalWrite(dPins[i],HIGH);
  }
  midi.update();
}

void loop() {
  midi.update();
  // Check for key press and release
  for(i=0;i<NumDigital;i++) {
    sdPins[i] = 0;
    dVal[i] = digitalRead(dPins[i]);
    if (millis () - tdPins[i] >= debounceTime) {
      tdPins[i] = millis();
      if (dVal[i] != ldPins[i]) {
         if(dVal[i] == HIGH) {
           ldPins[i] = HIGH;
           sdPins[i] = 1;
         } else {
           ldPins[i] = LOW;
           sdPins[i] = 1;
         }
      }
    }
  }

  // Send Midi messages for press and release
  for(i=0;i<NumDigital;i++) {
    if(sdPins[i]) {
      if(ldPins[i] == HIGH) {
          midi.sendNoteOff(dNotes[i],127);
      } else {
          midi.sendNoteOn(dNotes[i],127);
      }
    }
  }

  // Check analog read out
  for(i=0;i<NumAnalog;i++) {
    aVal[i] = 0x7f - analogRead(aPins[i])/32 * 4;
    
    if(aVal[i] != laPins[i]) {
      midi.sendControlChange(aNotes[i],aVal[i]);
      laPins[i] = aVal[i];
    }
  }
}


