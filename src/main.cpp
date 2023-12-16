#include <Arduino.h>
#include <MIDIUSB.h>
#include <pitchToNote.h>

#define MIDI_EVENT_NOTE_ON 8
#define MIDI_EVENT_NOTE_OFF 9
#define MIDI_CHANNEL 0
#define MIDI_VELOCITY 127  // Note volume from 0-127

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
 

static inline void sendMidiEvent(uint8_t event, uint8_t channel, uint8_t pitch, 
                                 uint8_t velocity) {
    midiEventPacket_t ev = {event, (event << 4) | channel, pitch, velocity};
    MidiUSB.sendMIDI(ev);
}

static volatile uint32_t PIN_STATE_SAVE = 0;

static void updateMidiState(void) {
    noInterrupts();
    uint32_t current_pin_states = 0;
    uint32_t previous_pin_states = PIN_STATE_SAVE;
    for (size_t i=0; i < NUMBER_OF_KEYS; i++) {
        uint8_t pin = PINS_AND_PITCHES[i].pin;
        uint8_t pitch = PINS_AND_PITCHES[i].pitch;
        uint32_t pin_mask = (uint32_t)1 << i;
        bool current_pin_state = digitalRead(pin);
        current_pin_states |= ((uint32_t)current_pin_state << i);
        bool previous_pin_state = previous_pin_states & pin_mask;
        if (previous_pin_state != current_pin_state) {
            if (current_pin_state) {
                sendMidiEvent(MIDI_EVENT_NOTE_ON, MIDI_CHANNEL, pitch, MIDI_VELOCITY);
            } else {
                sendMidiEvent(MIDI_EVENT_NOTE_OFF, MIDI_CHANNEL, pitch, MIDI_VELOCITY);
            }
        }    
    }
    MidiUSB.flush();
    PIN_STATE_SAVE = current_pin_states;
    interrupts();
}

void setup() {
    noInterrupts();
    for (size_t i=0; i < NUMBER_OF_KEYS; i++) {
        uint8_t pin = PINS_AND_PITCHES[i].pin;
        pinMode(pin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(pin),updateMidiState, CHANGE);
    }
    interrupts();
}

void loop(void) {
    while (true) {
        // Wait for Interrupt. This should put the arduino Due in sleep mode 
        // until an interrupt occurs. Saving power. Wakeup from sleep mode for 
        // The Atmel SAM3/A should be about 10 microseconds. The acceptable 
        // latency for pedal touch to MIDI event is about 1ms 
        // (1000 microseconds), so the wakeup time is not problematic.  
        __WFI();
    }
}
