#ifndef AVR_PLUS_PLUS_PORT_INCLUDED
#define AVR_PLUS_PLUS_PORT_INCLUDED

#include <avr/sfr_defs.h>
#include <stdint.h>

template <int IN_ADR, int DDR_ADR, int PORT_ADR, int PIN_MASK>
struct port {
    enum {
        in_adr  = IN_ADR,
        ddr_adr = DDR_ADR,
        port_adr = PORT_ADR,
        pins_present = PIN_MASK
    };

    static inline
    void
    enable_pullups(uint8_t mask) {
        _SFR_IO8(port_adr) |= mask;
    }
    static inline
    void
    disable_pullups(uint8_t mask) {
        _SFR_IO8(port_adr) &= ~mask;
    }
    static inline
    uint8_t
    read() {
        return _SFR_IO8(in_adr);
    }
};

#endif // AVR_PLUS_PLUS_PORT_INCLUDED
