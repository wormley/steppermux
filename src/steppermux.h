#include "generated_pins.h"
// These must be hard interrupt capable
//X-Min, X-Max, Z-Min, Z-Max

#define A0PIN 3
#define A1PIN 2
#define A2PIN 18
#define AEPIN 19
// Y pins are pin-change only, use step+direction here
// This is Y-Min
#define ASPIN 14
#define ASINPORT PINJ
#define ASBIT 1
// This is Y-Max
#define ADPIN 15
#define ADINPORT PINJ
#define ADBIT 0
#define ADPCINT 9
#define ADPCMSK PCMSK1
#define ADPCMSKBIT 1
#define ADPCICRBIT 1


// These are 'digitalPin' numbers
// the ISR sets enable, then the main loop 
// notices and sends steps to that device
unsigned char ena[] = {38,A2,A8,24,30,0,0,0};
unsigned char dir[] = {A1,A7,48,28,34,0,0,0};

// RAMPS X 
#define S0EPIN 38
#define S0SPIN A0
#define S0DPIN A1
// RAMPS Y
#define S1EPIN A2
#define S1SPIN A6
#define S1DPIN A7
// RAMPS Z
#define S2EPIN A8
#define S2SPIN 46
#define S2DPIN 48
// RAMPS E0
#define S3EPIN 24
#define S3SPIN 26
#define S3DPIN 28
// RAMPS E1
#define S4EPIN 30
#define S4SPIN 36
#define S4DPIN 34


#define REAL_GET_PORTR(x) GET_PORTR_ ## x
#define GET_PORTR(x) REAL_GET_PORTR(x)
#define REAL_GET_PINR(x) GET_PINR_ ## x
#define GET_PINR(x) REAL_GET_PINR(x)
#define REAL_GET_BIT(x) GET_BIT_ ## x
#define GET_BIT(x) REAL_GET_BIT(x)
#define REAL_GET_PIN(z) z ## PIN
#define GET_PIN(z) REAL_GET_PIN(z)

#define CLEAR(z) GET_PORTR(GET_PIN(z)) &= 255 - (1 << GET_BIT(GET_PIN(z)))
#define SET(z) GET_PORTR(GET_PIN(z)) |= 1 << GET_BIT(GET_PIN(z))

#define CHECKPIN(z) GET_PINR(GET_PIN(z)) & ~( 1 << GET_BIT(GET_PIN(z)) )
#define CHECKPORT(z) GET_PORTR(GET_PIN(z)) & ~( 1 << GET_BIT(GET_PIN(z)) )

#define STEPIT(x,y) if (x ## INPORT & 1 << x ## BIT  ) {SET(y); } else { CLEAR(y); } 
#define STEPITINV(x,y) if (x ## INPORT & 1 << x ## BIT  ) {CLEAR(y); } else { SET(y); } 
#define SETUPPIN(x) pinMode(x ## EPIN, OUTPUT);pinMode(x ## SPIN, OUTPUT);pinMode(x ## DPIN, OUTPUT)
// While the input is high, wait, when it goes low, clear the bit and wait for it to go high
// then set the bit, lather, rinse, repeat
#define STEPITB(x,y) while (x ## INPORT & 1 << x ## BIT) {}; CLEAR(y); while (!(x ## INPORT & 1 << x ## BIT)) {}; SET(y);
#define STEPITBINV(x,y) while (x ## INPORT & 1 << x ## BIT) {}; SET(y); while (!(x ## INPORT & 1 << x ## BIT)) {}; CLEAR(y);
// Test adding a check for enable here
#define STEPITC(x,y,z) while (x ## INPORT & 1 << x ## BIT) {if (! CHECKPORT(z)) goto MAINLOOP; }; CLEAR(y); while (!(x ## INPORT & 1 << x ## BIT)) {if (!CHECKPORT(z)) goto MAINLOOP; }; SET(y);
#define STEPITCINV(x,y,z) while (x ## INPORT & 1 << x ## BIT) {if (! CHECKPORT(z)) goto MAINLOOP; }; SET(y); while (!(x ## INPORT & 1 << x ## BIT)) {if (!CHECKPORT(z)) goto MAINLOOP; }; CLEAR(y);


#define PLOOP(x) if(GET_PORTR(GET_PIN(x ## E)) & 1 << GET_BIT(GET_PIN(x ## E))) for(;;) { STEPIT(AS , x ## S);}
#define PLOOPINV(x) if(GET_PORTR(GET_PIN(x ## E)) & 1 << GET_BIT(GET_PIN(x ## E))) for(;;) { STEPITINV(AS , x ## S);}

#define PLOOPB(x) if(GET_PORTR(GET_PIN(x ## E)) & 1 << GET_BIT(GET_PIN(x ## E))) {STEPIT(AS,x ## S); for(;;) { STEPITB(AS , x ## S);}}
#define PLOOPBINV(x) if(GET_PORTR(GET_PIN(x ## E)) & 1 << GET_BIT(GET_PIN(x ## E))) { STEPITINV(AS, x ## S); for(;;) { STEPITBINV(AS , x ## S);} }
#define PLOOPC(x) if(GET_PORTR(GET_PIN(x ## E)) & 1 << GET_BIT(GET_PIN(x ## E))) {STEPIT(AS,x ## S); for(;;) { STEPITC(AS , x ## S, x ## E);}}
#define PLOOPCINV(x) if(GET_PORTR(GET_PIN(x ## E)) & 1 << GET_BIT(GET_PIN(x ## E))) { STEPITINV(AS, x ## S); for(;;) { STEPITCINV(AS , x ## S, x ## E);} }
