#ifndef AVR_PLUS_PLUS_ATMEGA32U4_INCLUDED
#define AVR_PLUS_PLUS_ATMEGA32U4_INCLUDED

#include "port.hpp"

typedef port<0x03, 0x04, 0x05, 0xff> portb;
typedef port<0x06, 0x07, 0x08, 0xc0> portc;
typedef port<0x09, 0x0a, 0x0b, 0xff> portd;
typedef port<0x0c, 0x0d, 0x0e, 0x44> porte;
typedef port<0x0f, 0x10, 0x11, 0xf3> portf;

//struct clock_prescaler
#endif // AVR_PLUS_PLUS_ATMEGA32U4_INCLUDED
