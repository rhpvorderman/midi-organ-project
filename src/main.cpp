#include <Arduino.h>
#include <MIDIUSB.h>
#include <pitchToNote.h>

struct PinAndPitch {
  uint8_t pin;
  uint8_t pitch;
};

#define NUMBER_OF_KEYS 30

static const struct PinAndPitch PINS_AND_PITCHES[NUMBER_OF_KEYS] = {
  {30, pitchC1},
  {28, pitchD1b},
  {26, pitchD1},
  {24, pitchE1b},
  {22, pitchE1},
  {20, pitchF1},
  {21, pitchG1b},
  {23, pitchG1},
  {25, pitchA1b},
  {27, pitchA1},
  {29, pitchB1b},
  {31, pitchB1}, 
  {33, pitchC2},
  {35, pitchD2b},
  {37, pitchD2},
  {39, pitchE2b},
  {41, pitchE2},
  {43, pitchF2},
  {45, pitchG2b},
  {47, pitchG2},
  {49, pitchA2b},
  {51, pitchA2},
  {53, pitchB2b},
  {52, pitchB2},
  {50, pitchC3},
  {48, pitchD3b},
  {46, pitchD3},
  {44, pitchE3b},
  {42, pitchE3},
  {40, pitchF3},
  // G3b and G3 not present on pedal.
  // {38, pitchG3b},
  // {36, pitchG3},
};
 

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
