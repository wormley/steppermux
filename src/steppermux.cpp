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
volatile void Nothing() {};

volatile void S0Dchange() {
    STEPIT(AD,S0D);
};
volatile void S1Dchange() {
    STEPIT(AD,S1D);
};
volatile void S2Dchange() {
    STEPIT(AD,S2D);
};
volatile void S3Dchange() {
    STEPIT(AD,S3D);
};
volatile void S4Dchange() {
    STEPIT(AD,S4D);
};
volatile void (*dir[])()= {S0Dchange,S1Dchange,S2Dchange,S3Dchange,S4Dchange,Nothing,Nothing,Nothing};
volatile void (*selectedfunc)() = Nothing;


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

 void inline stepperchange() {

    // We do this the lazy way
    // Clear all pending interrupts, we check all bits and don't care
    // if multiple pins changed
    PCIFR = 255;
    // globally clear enable(well, set, Enable is active Low)
    // The main loop will properly handle this... maybe
    SET(S0E);
    SET(S1E);
    SET(S2E);
    SET(S3E);
    SET(S4E);
    // Not enabled, exit now
    if ( unsafedigitalRead(AEPIN) ) {selected=-1 ;selectedfunc = Nothing; return; }
    // enable on the one wanted port
selected = unsafedigitalRead(A0PIN) + (unsafedigitalRead(A1PIN) << 1) + (unsafedigitalRead(A2PIN) << 2);
    if (ena[selected] != 0 ) unsafedigitalWrite(ena[selected],0);
    selectedfunc=dir[selected];
    return;
};

void stepperchangewrap() {
    stepperchange();
// Yes, this is an ISR. Yes this is a longjmp() out of an ISR
// Rather than wasting cycles on a state variable, we force the main
// loop to effectively 'reset' and re-evaluate which pin it should
// be updating
    longjmp(back_to_loop,2);

}


void intSetup(byte pin) {
attachInterrupt(digitalPinToInterrupt(pin), stepperchangewrap, CHANGE);
};


//ISR (PCINT0_vect) 
// {dirchange();    
//};
 
ISR (PCINT1_vect) 
 {
     (*selectedfunc)();
 } ; 
 
//ISR (PCINT2_vect)
// {
//     dirchange();
// };  


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


TIMSK0=0;
TIMSK1=0;
TIMSK2=0;
TIMSK3=0;
TIMSK4=0;
TIMSK5=0;
};

void loop() {
// Call stepper change here for initial setup
    stepperchange();

    // This comes from the motor switch ISR to reset the
    // loop and re-evaluate the active stepper
    setjmp(back_to_loop);
    for(;;){
// Just in case our setjmp/longjmp doesn't do this
        interrupts();
// Set the direction pin on initial startup or if we
// were 'reset' due to motor change
    (*selectedfunc)();

PLOOPB(S0);
PLOOPB(S1);
PLOOPB(S2);
PLOOPB(S3);
PLOOPB(S4);

    };

};