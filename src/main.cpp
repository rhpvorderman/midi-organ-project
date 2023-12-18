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
 * 3. All pins trigger an interrupt. 
 * 4. The interrupt routine checks the pin states and if something is changed
 *    sends a midi event. 
 * 5. The main loop puts the microcontroller in sleep mode. It waits for
 *    interrupts and does not use power.
 * 
 * Since the interrupt routine can only run one at a time, there is a 
 * possibility that the latency for multiple keypresses becomes too much. 
 * Given that the wakeup from sleep mode takes about 10 microseconds, this 
 * should not be the case. 
 * 
 * The advantage of this method that only power is used when pins change state. 
 * So pressing down a key for two seconds only uses power for the press and the 
 * letting go. A polling method where each key is checked every x milliseconds, 
 * does use power on every poll.
 */

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

static volatile uint32_t PIN_STATE_SAVE = 0;

/**
 * @brief check which keys are pressed and send a MIDI event if a state has
 * has changed. 
 * The code is designed to run as interrupt routine. It blocks al incoming 
 * interrupts while running.
 */
static void updateMidiState(void) {
    noInterrupts();
    uint32_t current_pin_states = 0;
    uint32_t previous_pin_states = PIN_STATE_SAVE;
    for (uint32_t i=0; i < NUMBER_OF_KEYS; i++) {
        uint8_t pin = PINS_AND_PITCHES[i].pin;
        uint8_t pitch = PINS_AND_PITCHES[i].pitch;
        uint32_t pin_mask = (uint32_t)1 << i;
        // Input pullup makes pins LOW when the key is pressed.
        bool current_pin_state = digitalRead(pin) == LOW;
        bool previous_pin_state = previous_pin_states & pin_mask;
        if (previous_pin_state != current_pin_state) {
            if (current_pin_state) {
                current_pin_states |= pin_mask;
                sendMidiEvent(MIDI_EVENT_NOTE_ON, MIDI_CHANNEL, pitch, MIDI_VELOCITY);
            } else {
                sendMidiEvent(MIDI_EVENT_NOTE_OFF, MIDI_CHANNEL, pitch, MIDI_VELOCITY);
            }
        }    
    }
    if (current_pin_states) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    // USB is not realtime. Make sure the event arrives ASAP.
    MidiUSB.flush();
    PIN_STATE_SAVE = current_pin_states;
    interrupts();
}

/**
 * @brief Set all pins to pullup mode and make sure they can interrupt.
 * 
 */
void setup() {
    noInterrupts();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    for (size_t i=0; i < NUMBER_OF_KEYS; i++) {
        uint8_t pin = PINS_AND_PITCHES[i].pin;
        pinMode(pin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(pin),updateMidiState, CHANGE);
    }
    interrupts();
}

void loop(void) {
    while (true) {
        __WFI();
    }
}
