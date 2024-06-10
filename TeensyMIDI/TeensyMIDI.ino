#include <Bounce.h>  // Bounce library makes button change detection easy

// LED Output
const int led_output = 8;

// MIDI Channel used for all inputs
const int channel = 1;

// Default velocity used for all inputs 
const int default_velocity = 80;

typedef enum { 
  NOT_DETECTED, 
  DETECTED, 
  DEBOUNCE, 
  WAIT_FOR_RELEASE 
} input_state_t;

typedef enum { 
  ANALOG, 
  DIGITAL,
  DEACTIVATED 
} input_mode_t;

#define NUMBER_OF_INPUTS 6

const int threshold        = 80; 
const int hysteresis       = 40;
const int debounce_time_ms = 20;

// Inputs
// A2  : 16
// A3  : 17
// A6  : 20
// A7  : 21
// A10 : xx
// A11 : xx
//  _ _ _ _ _   _ _ _ _ _   _ _ _ _ _ 
// |         | |         | |         |
// |         | |   A2/   | |  A11/   | -> !buggy!
// |         | |    16   | |    xx   |
// |_ _ _ _ _| |_ _ _ _ _| |_ _ _ _ _|          __
//  _ _ _ _ _   _ _ _ _ _   _ _ _ _ _     **   |__|
// |         | |         | |         |
// |   A7/   | |         | |   A10/  |
// |    21   | |         | |    xx   |
// |_ _ _ _ _| |_ _ _ _ _| |_ _ _ _ _|

const int     input_pin[NUMBER_OF_INPUTS]    = {16, 17, 20, 21, A10, A11};
const int     input_mode[NUMBER_OF_INPUTS]   = {DIGITAL, DIGITAL, DIGITAL, DIGITAL, ANALOG, DEACTIVATED};
const int     note[NUMBER_OF_INPUTS]         = {36, 36, 36, 41, 36, 36};  // C1, For Sitala

// For digital 
Bounce *input_button[NUMBER_OF_INPUTS];

// For Analog
input_state_t input_state[NUMBER_OF_INPUTS]  = {NOT_DETECTED, NOT_DETECTED, NOT_DETECTED, NOT_DETECTED, NOT_DETECTED, NOT_DETECTED};
int           trigger_time[NUMBER_OF_INPUTS] = {0, 0, 0, 0, 0, 0};

void setup() {
  pinMode(led_output, OUTPUT);
  
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  input_button[0] = new Bounce(16, debounce_time_ms);
  input_button[1] = new Bounce(17, debounce_time_ms);
  input_button[2] = new Bounce(20, debounce_time_ms);
  input_button[3] = new Bounce(21, debounce_time_ms);

  // Boot LED sequence
  for(int i=0; i<4; i++) {
    digitalWrite(led_output, HIGH);
    delay(100);
    digitalWrite(led_output, LOW);
    delay(100);
  }
}

void loop() {
  for (int i=0; i < NUMBER_OF_INPUTS; i++)
  {
    if (input_mode[i] == ANALOG)
    {
      unsigned int value = analogRead(input_pin[i]);
      if (value > threshold) {
        switch (input_state[i]) {
          case NOT_DETECTED: 
            // Trigger detected
            input_state[i] = DETECTED;
            usbMIDI.sendNoteOn(note[i], map(value, threshold, 120, 7, 16)*8, channel);
            digitalWrite(led_output, HIGH);
            trigger_time[i] = millis();
            break;
          case DETECTED:
          case DEBOUNCE:
          case WAIT_FOR_RELEASE:
            // Do nothing
            break;
          default:
            // Error, deactivate input via MIDI
            usbMIDI.sendNoteOff(note[i], 0, channel);
            digitalWrite(led_output, LOW);
        }
      }
    
      if ((millis() - trigger_time[i]) > debounce_time_ms) {
        switch (input_state[i]) {
          case DETECTED: 
            // Debounce over
            input_state[i] = WAIT_FOR_RELEASE;
            break;
          case DEBOUNCE:
          case WAIT_FOR_RELEASE:
          case NOT_DETECTED:
            // Do nothing
          default:
            // Error, deactivate input via MIDI
            usbMIDI.sendNoteOff(note[i], 0, channel);
            digitalWrite(led_output, LOW);
        }
      }
    
      if (analogRead(input_pin[i]) < (threshold - hysteresis)) {
        switch (input_state[i]) {
          case WAIT_FOR_RELEASE: 
            // Trigger detected
            input_state[i] = NOT_DETECTED;
            usbMIDI.sendNoteOff(note[i], 0, channel);
            digitalWrite(led_output, LOW);
            break;
          case DETECTED:
          case DEBOUNCE:
          case NOT_DETECTED:
            // Do nothing
            break;
          default:
            // Error, deactivate input via MIDI
            usbMIDI.sendNoteOff(note[i], 0, channel);
            digitalWrite(led_output, LOW);
        }
      }
    }
    else if (input_mode[i] == DIGITAL)
    {
      input_button[i]->update();

      if (input_button[i]->fallingEdge()) 
      {
        usbMIDI.sendNoteOn(note[i], default_velocity, channel);
        digitalWrite(led_output, HIGH);
      }
      
      if (input_button[i]->risingEdge()) 
      {
        usbMIDI.sendNoteOff(note[i], 0, channel);
        digitalWrite(led_output, LOW);
      }
    }
  }
  // MIDI Controllers should discard incoming MIDI messages.
  while (usbMIDI.read()) {
  }
}
