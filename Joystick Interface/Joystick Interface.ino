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

   Teensy becomes a USB joystick with 16 or 32 buttons and 6 axis input

   You must select Joystick from the "Tools > USB Type" menu

   Pushbuttons should be connected between the digital pins and ground.
   Potentiometers should be connected to analog inputs 0 to 5.

   This example code is in the public domain.
*/
#include "usb_desc.h"

#if JOYSTICK_SIZE != 64
  #error "JOYSTICK_SIZE must be set to 64 in order to use this software. See notes above!"
#endif

// Configure the number of buttons.  Be careful not
// to use a pin for both a digital button and analog
// axis.  The pullup resistor will interfere with
// the analog voltage.
const int numButtons = 16;  // 16 for Teensy, 32 for Teensy++

void setup() {
  // you can print to the serial monitor while the joystick is active!
  Serial.begin(9600);
  // configure the joystick to manual send mode.  This gives precise
  // control over when the computer receives updates, but it does
  // require you to manually call Joystick.send_now().
  Joystick.useManualSend(true);
  for (int i=0; i<numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  
  analogReadResolution(12); // configure the analog ports for 12 bit resolution.
  Serial.println("Begin Complete Joystick Test");
}

byte allButtons[numButtons];
byte prevButtons[numButtons];
int angle=0;

void loop() {
  // read 6 analog inputs and use them for the joystick axis
  Joystick.X(analogRead(0));
  Joystick.Y(analogRead(1));
  Joystick.Z(analogRead(2));
  Joystick.Zrotate(analogRead(3));
  //Joystick.sliderLeft(analogRead(4));
  //Joystick.sliderRight(analogRead(5));
  Joystick.Xrotate(analogRead(6));
  Joystick.Yrotate(analogRead(7));

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

