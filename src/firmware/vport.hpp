#ifndef AVR_PLUS_PLUS_VPORT_INCLUDED
#define AVR_PLUS_PLUS_VPORT_INCLUDED

struct not_a_pin {
    static inline
    uint8_t
    read() { return 0; }

    static inline
    void
    enable_pullup() {}

    enum { bit = 0 };
};

template <
    typename Pin0 = not_a_pin
  , typename Pin1 = not_a_pin
  , typename Pin2 = not_a_pin
  , typename Pin3 = not_a_pin
  , typename Pin4 = not_a_pin
  , typename Pin5 = not_a_pin
  , typename Pin6 = not_a_pin
  , typename Pin7 = not_a_pin
>
struct vport {
    static inline
    uint8_t
    read() {
        return ((Pin0::read() >> Pin0::bit) << 0)
            | ((Pin1::read() >> Pin1::bit) << 1)
            | ((Pin2::read() >> Pin2::bit) << 2)
            | ((Pin3::read() >> Pin3::bit) << 3)
            | ((Pin4::read() >> Pin4::bit) << 4)
            | ((Pin5::read() >> Pin5::bit) << 5)
            | ((Pin6::read() >> Pin6::bit) << 6)
            | ((Pin7::read() >> Pin7::bit) << 7)
            ;
    }
    static inline
    void
    enable_pullups() {
        Pin0::enable_pullup();
        Pin1::enable_pullup();
        Pin2::enable_pullup();
        Pin3::enable_pullup();
        Pin4::enable_pullup();
        Pin5::enable_pullup();
        Pin6::enable_pullup();
        Pin7::enable_pullup();
    }
    static inline
    void
    enable_pullups(uint8_t mask) {
        if (mask & _BV(0)) Pin0::enable_pullup();
        if (mask & _BV(1)) Pin1::enable_pullup();
        if (mask & _BV(2)) Pin2::enable_pullup();
        if (mask & _BV(3)) Pin3::enable_pullup();
        if (mask & _BV(4)) Pin4::enable_pullup();
        if (mask & _BV(5)) Pin5::enable_pullup();
        if (mask & _BV(6)) Pin6::enable_pullup();
        if (mask & _BV(7)) Pin7::enable_pullup();
    }
};

#endif // AVR_PLUS_PLUS_VPORT_INCLUDED
