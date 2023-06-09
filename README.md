# Teensy Joystick Interface

The Teensy line of microcontroller development boards lend themselves nicely
to those of us insane enough to want to build our own flight simulators.

The reason for this is how trival they can be used to enumerate as a number of
different USB devices to the host computer they're connected to.

The genesis for this particular spin on that idea is based on the discussions
in this thread on the PJRC forum: https://forum.pjrc.com/threads/23681-Many-axis-joystick
While the thread dates back to 2013, it's still relevant - for the most part.  The
changes that Mr. Stoffregen lists to the usb_* files in post #36 are no longer required - 
the changes are part of the main Teensy library package now.  Note that this only applies
to the Teensy 3.x and the Teensy 4.x.

My starting point is the ExtremeJoystickTest.ino file in post #36, but with a number of 
changes.

The goal of this project is to have a decent USB interface to the flight controls in my
F-15C flight simulator project (https://www.f15sim.com).

The first stage is to get a working example that will cover the seven analog inputs that
my project requires - roll, pitch, yaw, left & right toe brakes, and left & right throttle
inputs.

The hardware I'll be using is the Teensy 3.1.  This is due to me having purchased the 
hardware back in 2014 and then not getting back to finishing the project after it arrived.
For those that are getting into this fresh, I recommend looking at the Teensy 4.x line as
the 3.x boards are likely never to be made available again due to parts shortages[*]. 

[*] as of May, 2023.



