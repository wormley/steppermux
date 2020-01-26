#include <Arduino.h>
#include <setjmp.h>
#include "steppermux.h"
volatile int8_t selected=-1;

jmp_buf back_to_loop;
int main(){
    setup();
    for(;;) loop();
    return(0);
};
// TODO
// Startup init for enable
// Change for init for direction


void unsafedigitalWrite(uint8_t pin, uint8_t val)
{
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out;
	out = portOutputRegister(port);
	if (val == LOW) {
		*out &= ~bit;
	} else {
		*out |= bit;
	}
}

int unsafedigitalRead(uint8_t pin)
{
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	if (*portInputRegister(port) & bit) return HIGH;
	return LOW;
}


void pciSetupDirection()
{
    ADPCMSK = 1 << ADPCMSKBIT;  // enable pin
    PCIFR  |= 1 << ADPCICRBIT; // clear any outstanding interrupt
    PCICR  |= 1 << ADPCICRBIT; // enable interrupt for the group
}
 void stepperchange() {

    // We do this the lazy way
    // Clear all pending interrupts, we check all bits and don't care
    // if multiple pins changed
    PCIFR = 255;
    // globally clear enable
    // The main loop will properly handle this... maybe
    CLEAR(S0E);
    CLEAR(S1E);
    CLEAR(S2E);
    CLEAR(S3E);
    CLEAR(S4E);
    // Not enabled, exit now
    if ( ! unsafedigitalRead(AEPIN) ) {selected=-1 ; longjmp(back_to_loop,1); }
    // enable on the one wanted port
selected = unsafedigitalRead(A0PIN) + (unsafedigitalRead(A1PIN) << 1) + (unsafedigitalRead(A2PIN) << 2);
    if (ena[selected] != 0 ) unsafedigitalWrite(ena[selected],1);
    longjmp(back_to_loop,2);
};
void dirchange() {
    if (selected >= 0 && dir[selected] >0 )
    unsafedigitalWrite(dir[selected],unsafedigitalRead(ADPIN));
};

void intSetup(byte pin) {
attachInterrupt(digitalPinToInterrupt(pin), stepperchange, CHANGE);
};


ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
 {dirchange();    
};
 
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
 {
     dirchange();
 } ; 
 
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
 {
     dirchange();
 };  


void setup() {
pinMode(AEPIN,INPUT_PULLUP) ;

pinMode(A0PIN,INPUT_PULLUP) ;
pinMode(A1PIN,INPUT_PULLUP) ;
pinMode(A2PIN,INPUT_PULLUP) ;

pinMode(ASPIN,INPUT_PULLUP) ;
pinMode(ADPIN,INPUT_PULLUP) ;
// Direction pin we use a change interrupt
pciSetupDirection();
// Other pins we use a hard interrupt
intSetup(AEPIN);
intSetup(A0PIN);
intSetup(A1PIN);
intSetup(A2PIN);


SETUPPIN(S0);
SETUPPIN(S1);
SETUPPIN(S2);
SETUPPIN(S3);
SETUPPIN(S4);

};
void loop() {
    setjmp(back_to_loop);
    MAINLOOP: for(;;){
        interrupts();
PLOOPBINV(S0);
PLOOPB(S1);
PLOOP(S2);
PLOOPC(S3);
PLOOPCINV(S4);
    };

};