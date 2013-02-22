#ifndef AVR_PLUS_PLUS_INCLUDED
#define AVR_PLUS_PLUS_INCLUDED

#if defined (__AVR_ATmega32U2__)
# include "ATmega32U2.hpp"
#elif defined (__AVR_ATmega32U4__)
# include "ATmega32U4.hpp"
#else
# warning "device type not defined"
#endif

#include "pin.hpp"
#include "port.hpp"
#include "vport.hpp"

#endif // AVR_PLUS_PLUS_INCLUDED

