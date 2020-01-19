# steppermux
Multiplex Stepper Drivers

Takes a 1 to 3 bit input with Step, Direction and Enable. Uses it to feed 1-8 stepper drivers.

Arduino flavored. Should work with RAMPS style boards, pins will need to be updated.

The pins in generated_pins.h are extracted from the Arduino pins file so that everything
can be done as a define.

Main step loop seems fast enough, not yet sure about the delay on the Direction change.

