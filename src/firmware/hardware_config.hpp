#ifndef HARDWARE_CONFIG_INCLUDED
#define HARDWARE_CONFIG_INCLUDED

#include "usb_config.h"

#include "avr++.hpp"

#include <avr/io.h> // required for FLASHEND

//==============================================================================
// Main Software Clock
//==============================================================================

#define MAIN_TIMER_INTERRUPT TIMER0_COMPA_vect
struct main_timer {
    enum {
          frequency = 10000UL, // Hz
          prescale = 8
    };
};

//==============================================================================
// Periphery: USB <-> RS232 Converter
//==============================================================================

struct usart2usb_config {
    enum {
        queue_size = 32,
        high_water_mark = 24,
        flush_frequency = 100 // Hz
    };
};

enum {
    bootsector_size = 4096,
    bootloader_address = FLASHEND - bootsector_size,
    bootloader_magic_key = 0xACB00f
};

#endif // HARDWARE_CONFIG_INCLUDED
