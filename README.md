# steppermux
Multiplex Stepper Drivers using RAMPS and Mega2560

Takes a 1 to 3 bit input with Step, Direction and Enable. Uses it to feed 1-8 stepper drivers.

This should be compatible with Marlin's stepper selection code. 
X-Min: bit0 , X-Max: bit 1, Z-Min: bit 2, Z-Max: enable(active low)

Currently all are pulled up, so unconnected bits(if you only need 2 or 4) 
should be pulled down.

If your RAMPS adds capacitors to the endstop pins you'll almost certainly
need to remove them.


Arduino flavored. Should work with RAMPS style boards, pins will need to be updated.

The pins in generated_pins.h are extracted from the Arduino pins file so that everything
can be done as a define.

Timing:

Step loop takes between 400-700nS to change, so a 1uS minimum pulse and not-pulse time
should be about right, giving a maximum of about 500KHz

Direction change takes about 4uS. So 5uS before next pulse at a minimum, closer to 10uS 
is better.

Motor/enable switch, assume 1mS minimum, it's a long, convoluted routine.

Hardware:
Very hardcoded for Mega2560.(generated_pins.h along with interrupts chosen)

Somewhat hardcoded for RAMPS(need interrupt capable pins for selecting stepper,
enable as well as direction) Step is a loop and doesn't need interrupts.

Adding or changing outputs should be easy, edit .h for more pins
and every place S0 - S4 is referenced, add some more
(plus the 'ena' and 'dir' arrays)

Todo:
Add TMC2130 setup option
Add pins for Aux connector to be able to use full 8 stppers.
(Using off-board step stick carrier boards)



