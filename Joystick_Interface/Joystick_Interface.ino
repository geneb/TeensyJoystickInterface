/* This is based upon the "Complete" USB Joystick Example from PJRC.

  This example has been modified to use the diffent methods that are exposed
  when setting JOYSTICK_SIZE to 64.  JOYSTICK_SIZE is declared in usb_desc.h
  in the cores\<product> directory.  For example, if you're working with a Teensy 3.x
  and you're using Windows, the most likely location for the file will be:
  C:\Users\<userid>\AppData\Local\Arduino15\packages\teensy\hardware\avr\<version>\cores\teensy3\

  Note: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        !! When JOYSTICK_SIZE is set to 12, the methods that parse
        !! X(), Y(), Z(), position(), Zrotate(), sliderLeft(), sliderRight(),
        !! and slider() methods clamp the values passed to 10 bits (0..1023).
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  When JOYSTICK_SIZE is set to 64, analog values are clamped to 16 bits.

  This program was built with "Serial + Mouse + Keyboard + Joystick" selected
  from the Tools -> USB Type dropdown in the Arduino IDE.

*/
#include "usb_desc.h"  // for the JOYSTICK_SIZE test.
#include "EEPROM.h"

#if JOYSTICK_SIZE != 64
  #error "JOYSTICK_SIZE must be set to 64 in order to use this software. See notes above!"
#endif

#define BUTTON_NORM        0x01   // Normal on/off button input
#define BUTTON_MACRO_PRESS 0x02   // Button triggers a macro when pressed
#define BUTTON_MACRO_REL   0x04   // Button triggers a macro when released
#define BUTTON_SERIAL      0x08   // Used in combination with BUTTON_MACRO_* - output goes to the
                                  // serial port instead of the keyboard.
#define BUTTON_LATCHED     0x10   // Button latches state each press.  (press once to set on, again to turn off)

// Configure the number of buttons.  Be careful not
// to use a pin for both a digital button and analog
// axis.  The pullup resistor will interfere with
// the analog voltage.
const int numButtons = 12;  // will use the first 12 pins for buttons for now.
                            // in order to actually read up to 128 button inputs,
                            // an input matrix of some kind will be required.
                            // However, that's beyond the scope of my particular 
                            // project... (for those interested, the Keypad Library
                            // may be of some use)

byte buttonType[numButtons]; // stores special activity codes per button.

void setup() {
  Serial.begin(9600);
  // configure the joystick to manual send mode.  This gives precise
  // control over when the computer receives updates, but it does
  // require you to manually call Joystick.send_now().
  Joystick.useManualSend(true);

  // Initialize the pins used for buttons.
  for (int i=0; i<numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }
   
  analogReadResolution(12); // configure the analog ports for 12 bit resolution.
  loadButtonConfig();
}

byte allButtons[numButtons];
byte prevButtons[numButtons];
int angle=0;

void loop() {
  // read the analog inputs and use them for the joystick axis
  // The pin constants used correspond to what I'm using on the Teensy 3.1 board.

  Joystick.X(readAnalog(A0));  // Roll
  Joystick.Y(readAnalog(A1));  // Pitch
  Joystick.Z(readAnalog(A2));  // Yaw
  //Joystick.Zrotate();
  //Joystick.Xrotate();
  //Joystick.Yrotate();
  Joystick.slider(0, readAnalog(A3)); // Left toe brake
  Joystick.slider(1, readAnalog(A4)); // Right toe brake
  Joystick.slider(2, readAnalog(A5)); // Left Throttle
  Joystick.slider(3, readAnalog(A6)); // Right Throttle

  // read digital pins and use them for the buttons
  for (int i=0; i<numButtons; i++) {
    if (digitalRead(i)) {
      // when a pin reads high, the button is not pressed
      // the pullup resistor creates the "on" signal
      allButtons[i] = 0;
    } else {
      // when a pin reads low, the button is connecting to ground.
      allButtons[i] = 1;
    }

    if (buttonType[i] & BUTTON_NORM) {
        Joystick.button(i + 1, allButtons[i]);
    }
    if (buttonType[i] & BUTTON_MACRO_PRESS) {
        if (buttonType[i] & BUTTON_SERIAL) {
          // output goes to the serial port.
        } else {
          // output goes to the keyboard.
        }
    }
    if (buttonType[i] & BUTTON_MACRO_REL) {
        // check to see if the button is being released, if so
        // kick off the macro.
         if (buttonType[i] & BUTTON_SERIAL) {
          // output goes to the serial port.
        } else {
          // output goes to the keyboard.
        }
    }
    if (buttonType[i] & BUTTON_LATCHED) {
        // If this is the first time this button has been pressed,
        // "latch" it down and sent a button down to the host.
        // if this is the second time we've seen this button and 
        // it's latched, unlatch it and send a button up to the host.
    }
    Joystick.button(i + 1, allButtons[i]);
  }

  // make the hat switch automatically move in a circle
  angle = angle + 1;
  if (angle >= 360) angle = 0;
  Joystick.hat(0, angle);  // only affects a single hat.
  
  // Because setup configured the Joystick manual send,
  // the computer does not see any of the changes yet.
  // This send_now() transmits everything all at once.
  Joystick.send_now();
  
  // check to see if any button changed since last time
  boolean anyChange = false;
  for (int i=0; i<numButtons; i++) {
    if (allButtons[i] != prevButtons[i]) anyChange = true;
    prevButtons[i] = allButtons[i];
  }
  
  // if any button changed, print them to the serial monitor
  if (anyChange) {
    Serial.print("Buttons: ");
    for (int i=0; i<numButtons; i++) {
      Serial.print(allButtons[i], DEC);
    }
    Serial.println();
  }
  
  // a brief delay, so this runs "only" 200 times per second
  delay(5);
}

void loadButtonConfig() {
  // reads button configuration from EEPROM.
  if (EEPROM.read(0) == 0x49) {  // "I" - means the address space has been initialized in a previous run.
    for(int addr = 0; addr < numButtons; addr++) {
      buttonType[addr] = EEPROM.read(addr - + 1);
    }
  } else {
    initButtonConfig();
  }
}

void initButtonConfig() {
  EEPROM.write(0, 0x49); // "I"
  for(int addr=0; addr < numButtons; addr++) {
    EEPROM.write(addr + 1, BUTTON_NORM);
  }
}

int readAnalog(int channel) {
// This exists to allow for future "shaping" of the analog read values
// in the future.
  return analogRead(channel);
}