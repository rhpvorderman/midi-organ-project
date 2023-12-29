/**
 * @file main.cpp
 * @author Ruben Vorderman (rubenvorderman@xs4all.nl)
 * @brief Code to run a 30 pedal keyboard on an arduino due.
 * @date 2023-12-16
 * 
 * @copyright Copyright (c) 2023
 * 
 * The code works as follows
 * 1. Each pin is coupled to a midi pitch. 
 * 2. All pins are set to input pullup mode. 
 * 3. Pins are polled and MIDI events are sent. 
 * 4. A 1 ms wait is induced to prevent using too much power.
 */

#include <Arduino.h>
#include <MIDIUSB.h>
#include <pitchToNote.h>

#define MIDI_EVENT_NOTE_ON 9
#define MIDI_EVENT_NOTE_OFF 8
#define MIDI_CHANNEL 0
#define MIDI_VELOCITY 127  // Note volume from 0-127

struct PinAndPitch {
    uint8_t pin;
    uint8_t pitch;
};

#define NUMBER_OF_KEYS 30

/* Organ standard pedal range is from C2 up to G4 */
static const struct PinAndPitch PINS_AND_PITCHES[NUMBER_OF_KEYS] = {
    {30, pitchC2},
    {28, pitchD2b},
    {26, pitchD2},
    {24, pitchE2b},
    {22, pitchE2},
    {20, pitchF2},
    {21, pitchG2b},
    {23, pitchG2},
    {25, pitchA2b},
    {27, pitchA2},
    {29, pitchB2b},
    {31, pitchB2},
    {33, pitchC3},
    {35, pitchD3b},
    {37, pitchD3},
    {39, pitchE3b},
    {41, pitchE3},
    {43, pitchF3},
    {45, pitchG3b},
    {47, pitchG3},
    {49, pitchA3b},
    {51, pitchA3},
    {53, pitchB3b},
    {52, pitchB3},
    {50, pitchC4},
    {48, pitchD4b},
    {46, pitchD4},
    {44, pitchE4b},
    {42, pitchE4},
    {40, pitchF4},
    // G4b and G4 not present on pedal.
    // {38, pitchG4b},
    // {36, pitchG4},
};
 

/**
 * @brief Convenience function to send a MIDI event.
 * 
 * @param event 
 * @param channel 
 * @param pitch 
 * @param velocity 
 */
static inline void sendMidiEvent(uint8_t event, uint8_t channel, uint8_t pitch, 
                                 uint8_t velocity) {
    midiEventPacket_t ev = {event, (event << 4) | channel, pitch, velocity};
    MidiUSB.sendMIDI(ev);
}

uint32_t current_pin_states = 0;
uint32_t previous_pin_states = 0;

void setup() {
    for (size_t i=0; i < NUMBER_OF_KEYS; i++) {
        uint8_t pin = PINS_AND_PITCHES[i].pin;
        pinMode(pin, INPUT_PULLUP);
    }
    current_pin_states = 0;
    previous_pin_states = 0;
}


void loop(void) {
    previous_pin_states = current_pin_states;
    current_pin_states = 0;
    for (uint32_t i=0; i < NUMBER_OF_KEYS; i++) {
        uint8_t pin = PINS_AND_PITCHES[i].pin;
        uint8_t pitch = PINS_AND_PITCHES[i].pitch;
        uint32_t pin_mask = (uint32_t)1 << i;
        // Input pullup makes pins LOW when the key is pressed.

        bool current_pin_state = (digitalRead(pin) == LOW);
        bool previous_pin_state = previous_pin_states & pin_mask;
        current_pin_states |= ((uint32_t)current_pin_state << i);
        if (previous_pin_state != current_pin_state) {
            uint8_t midi_event = MIDI_EVENT_NOTE_OFF;
            uint8_t velocity = 0;
            if (current_pin_state) {
                midi_event = MIDI_EVENT_NOTE_ON;
                velocity = MIDI_VELOCITY;
            }
            sendMidiEvent(midi_event, MIDI_CHANNEL, pitch, velocity);
        }    
    }
    if (current_pin_states ^ previous_pin_states) {
        // If there is any difference, flush so events are sent ASAP.
        MidiUSB.flush();
    }
    delay(1);
}
