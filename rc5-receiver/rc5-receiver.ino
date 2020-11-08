#include <RC5.h>

// Define the RC5 command and address codes
#define address_CD 20
#define command_rev 33
#define command_fwd 32
#define command_stop 54
#define command_play 53
#define command_pause 48
#define command_mdax 130

// Define the I/O pin used for the IR receiver
#define IR_PIN 5

// Define the I/O pins used to control the cassette deck (remote control button names used here)
#define pin_rev 13
#define pin_fwd 12
#define pin_stop 11
#define pin_play 10
#define pin_pause 9
#define pin_mdax 8
#define pin_invalid 255

// Define the button press timings
#define quiet_time 140
#define pin_mark_time 100

// Define variables
uint_fast8_t toggle;
uint_fast8_t address;
uint_fast8_t command;
uint32_t lastPressTime;
uint_fast8_t lastCommand;
uint_fast8_t lastAddress;
uint_fast8_t lastToggle;
uint_fast8_t pinToUse;

// Initialise the RC5 receiver
RC5 rc5(IR_PIN);

void setup() {
  // Setup code to make it clear that the I/O pins are normally high-impedance
  // I/O pins default to this anyway so these commands don't really do anything
  pinMode(pin_rev, INPUT);
  pinMode(pin_fwd, INPUT);
  pinMode(pin_stop, INPUT);
  pinMode(pin_play, INPUT);
  pinMode(pin_pause, INPUT);
  pinMode(pin_mdax, INPUT);
  Serial.begin(57600);
}

void loop() {
  // Wait until an RC5 packet is received
  if (rc5.read(&toggle, &address, &command))
  {
    // Reject duplicate commands received when the button is held down, but not when two unique button
    // presses are made in a row. The toggle variable changes when a new button press is made, but not
    // when the button is held down continuously.
    if (!((lastCommand == command) && (lastAddress == address) && (lastToggle == toggle)
    && ((millis() - lastPressTime) < quiet_time))) {
      if (address = address_CD) {
        // Print recieved command in terminal (if connected)
        Serial.print(address);
        Serial.print(",");
        Serial.print(command);
        Serial.println(toggle ? " (t) " : "");
        // Select I.O pin to use according to command
        switch(command) {
          case command_rev:
          pinToUse = pin_rev;
          break;
          case command_fwd:
          pinToUse = pin_fwd;
          break;
          case command_stop:
          pinToUse = pin_stop;
          break;
          case command_play:
          pinToUse = pin_play;
          break;
          case command_pause:
          pinToUse = pin_pause;
          break;
          case command_mdax:
          pinToUse = pin_mdax;
          break;
          default:
          pinToUse = pin_invalid;
        }
        // Pulse the pin
        pulsePin(pinToUse);
      }
      lastCommand = command;
      lastAddress = address;
      lastToggle = toggle;
    }
    
    lastPressTime = millis();

  }
}

void pulsePin(uint_fast8_t pin) {
  if (pin != pin_invalid) {
    pinMode(pin, OUTPUT);     // Drive pin low
    delay(pin_mark_time);     // Wait a bit
    pinMode(pin, INPUT);      // Return pin to high impedance
  }
}

