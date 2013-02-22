#ifndef AVR_PLUS_PLUS_PIN_INCLUDED
#define AVR_PLUS_PLUS_PIN_INCLUDED

#include <stdint.h>
#include <avr/sfr_defs.h>

template <typename Port, uint8_t Bit>
struct pin {
    typedef Port port;
    enum { bit = Bit };
    static inline void make_output() {
        _SFR_IO8(port::ddr_adr) |= _BV(bit);
    }
    static inline void make_input() {
        _SFR_IO8(port::ddr_adr) &= ~_BV(bit);
    }
    static inline void enable_pullup() {
        _SFR_IO8(port::port_adr) |= _BV(bit);
    }
    static inline uint8_t read() {
        return _SFR_IO8(port::in_adr) & _BV(bit);
    }
    static inline bool is_high() {
        return read();
    }
    static inline void set_high() {
        _SFR_IO8(port::port_adr) |= _BV(bit);
    }
    static inline void set_low() {
        _SFR_IO8(port::port_adr) &= ~_BV(bit);
    }
    static inline void set(bool flag) {
        if (flag) {
            set_high();
        } else {
            set_low();
        }
    }

};

#endif // AVR_PLUS_PLUS_PIN_INCLUDED
