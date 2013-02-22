#include "hardware_config.hpp"

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include <stdlib.h>
#include <stdio.h>

#include <LUFA/Drivers/USB/USB.h>

#include "ringbuffer.hpp"

void bootloader_start();

FILE out;

enum {
    one_milli_second = 16050 /* F_CPU / 1000 */,
    center = one_milli_second / 2,
    servo_max = one_milli_second,
    step_size = 10
};

typedef pin<portb, 3> status_led;

int16_t x_position = center;
int16_t y_position = center;
int16_t angle = center;

volatile struct misc_flags {
    uint8_t enter_menu_mode: 1;
    uint8_t menu_mode: 1;
    uint8_t cli_mode: 1;
    uint8_t echo: 1;
} flags;

char command_buffer[64];
uint8_t command_buffer_index = 0;

uint8_t midi_channel_ee EEMEM = 0;
uint8_t midi_channel = 0;
//uint16_t v_rotary_offset_ee EEMEM;

extern "C" {
    extern USB_ClassInfo_CDC_Device_t cdc;
    extern USB_ClassInfo_MIDI_Device_t usb_midi;

    void EVENT_USB_Device_Connect(void);
    void EVENT_USB_Device_Disconnect(void);
    void EVENT_USB_Device_ConfigurationChanged(void);
    void EVENT_USB_Device_ControlRequest(void);
}

char menu_screen[] PROGMEM = 
"MikeBot Settings\r\n";

char config_json[] PROGMEM = 
"{ \"serialNumber\": \"%s\"\r\n"
", \"midiChannel\": %d\r\n"
", \"name\": \"%s\"\r\n"
", \"subtitle\": \"%s\"\r\n"
"}\r\n";

char config_command_string[] PROGMEM = "config";
char set_command_string[] PROGMEM = "set";
char set_error_argcount[] PROGMEM = "ERROR: set requires at least two arguments\r\n";
char set_error_unknown_parameter[] PROGMEM = "ERROR: unknown parameter '%s'\r\n";
char set_error_channel_out_of_range[] PROGMEM = "ERROR: channel out of range\r\n";

char midi_channel_string[] PROGMEM = "midiChannel";

inline
void
get_serial_number(char * buffer) {
    uint8_t sig_adr = INTERNAL_SERIAL_START_ADDRESS;
    for (uint8_t i = 0; i < (INTERNAL_SERIAL_LENGTH_BITS / 4); ++i) {
        uint8_t serial_byte = boot_signature_byte_get(sig_adr);
        if (i & 0x01) {
            serial_byte >>= 4;
            ++sig_adr;
        }
        serial_byte &= 0x0f;
        buffer[i] = serial_byte >= 10 ? 'A' + (serial_byte - 10) : '0' + serial_byte;
    }
    buffer[(INTERNAL_SERIAL_LENGTH_BITS / 4)] = '\0';
}

inline
void
config_command(char ** argv) {
    char serial_number[(INTERNAL_SERIAL_LENGTH_BITS / 4) + 1];
    get_serial_number(serial_number);
    fprintf_P(&out, config_json, serial_number, midi_channel, "MikeBot", "Marshall Left Channel");
}

inline
void
set_command(char ** argv) {
    uint8_t end = 1;
    while (argv[end] != NULL) {
        ++end;
    }
    if (end < 3) {
        fprintf_P(&out, set_error_argcount);
        return;
    }
    if (strcmp_P(argv[1], midi_channel_string) == 0) {
        int new_channel = atoi(argv[2]);
        if (new_channel < 0 || new_channel > 15) {
            fprintf_P(&out, set_error_channel_out_of_range, argv[1]);
            return;
        }
        if (new_channel != midi_channel) {
            midi_channel = new_channel;
            eeprom_write_byte(&midi_channel_ee, midi_channel);
        }
    } else {
        fprintf_P(&out, set_error_unknown_parameter, argv[1]);
    }
}

inline
bool
dtr_is_set(USB_ClassInfo_CDC_Device_t * cdc) {
    return cdc->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR;
}

inline
void
init_servos() {
#if defined(__AVR_ATmega32U4__)
    DDRB   |= _BV(DDB5)   | _BV(DDB6)   | _BV(DDB7);   // configure pins as outputs
#elif defined(__AVR_ATmega32U2__)
    DDRC   |= _BV(DDC5)   | _BV(DDC6);
    DDRB   |= _BV(DDB7);                               // configure pins as outputs
#else
# error Unhandled MCU
#endif
    TIMSK1 |= _BV(TOIE1);                              // enable overflow interrupt
    OCR1A = OCR1B = OCR1C = one_milli_second + center;
    TCNT1 = 0;
    TCCR1B |= _BV(CS10);                               // set prescaler to one
}

// setup timer 0 as a ~100Hz software clock
inline
void
init_software_clock() {
    TCCR0A |= _BV(WGM01);            // use ctc mode
    OCR0A = 156;                     // true 100Hz would be 156.25 TODO: replace with computed constant
    TIMSK0 |= _BV(OCIE0A);           // enable output compare interrupt
    TCCR0B |= _BV(CS02) | _BV(CS00); // set prescaler to 1024
}

inline
void 
init() {
    // Disable watchdog if enabled by bootloader/fuses
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

#ifdef CLKPR
    // Disable clock division
    CLKPR = _BV(CLKPCE);
    CLKPR = 0;
#endif

#if ((defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) ||  \
        defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) ||  \
        defined(__AVR_ATmega16U4__)  || defined(__AVR_ATmega32U4__) ||  \
        defined(__AVR_ATmega32U6__)))
    // Disable JTAG
    // Note: The JTD bit must be written twice within 4 clock cycles to disable JTAG.
    // You must also set the IVSEL bit at the same time, which requires IVCE to be set first.
    // Port pull-up resistors are enabled - PUD(Pull Up Disable) = 0
    MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD);
    MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD);
#endif


    USB_Init();

    CDC_Device_CreateStream(&cdc, &out);

    init_servos();
    init_software_clock();

    status_led::make_output();

    // Setup done. Enable interrupts.
    sei();

    midi_channel = eeprom_read_byte(&midi_channel_ee);
    if (midi_channel > 15) {
        midi_channel = 0;
    }
}

inline
void
handle_command_char(FILE * s, char c) {
    switch (c) {
        case 'b': // bootload: reset device and wait for new firmware
            bootloader_start();
            break;
        case 'C':
            flags.cli_mode = true;
            flags.echo = false;
            fprintf(s, "\r\n> ");
            break;
        case 'c':
            flags.cli_mode = true;
            flags.echo = true;
            fprintf(s, "\r\n> ");
            break;
        case 'x':
            x_position = MAX(x_position - step_size, 0);
            break;
        case 'X':
            x_position = MIN(x_position + step_size, servo_max);
            break;
        case 'y':
            y_position = MAX(y_position - step_size, 0);
            break;
        case 'Y':
            y_position = MIN(y_position + step_size, servo_max);
            break;
        case 'a':
            angle = MAX(angle - step_size, 0);
            break;
        case 'A':
            angle = MIN(angle + step_size, servo_max);
            break;
        default:
            // unknown command
            fprintf(s, "e 255 unknown command '%c'\r\n", c);
    }
}

void
parse_command(char * cmd) {

    char **ap, *argv[10];

    for (ap = argv; (*ap = strsep(& cmd, " \t")) != NULL;) {
        if (**ap != '\0') {
            if (++ap >= &argv[10]) {
                break;
            }
        }
    }
    if (strcmp_P(argv[0], config_command_string) == 0) {
        config_command(argv);
    } else if (strcmp_P(argv[0], set_command_string) == 0) {
        set_command(argv);
    } else {
        fprintf(&out, "unknown command '%s'\r\n", argv[0]);
    }
}

int main() {
    init();
    while (true) {
        status_led::set(dtr_is_set(&cdc));

        if (! flags.menu_mode && flags.enter_menu_mode) {
            flags.menu_mode = true;
            flags.enter_menu_mode = false;
            fputs_P(menu_screen, &out);
        }

        if (flags.menu_mode) {
        }

        int16_t received_byte = CDC_Device_ReceiveByte(&cdc);
        if (received_byte >= 0) {
            if (flags.cli_mode) {
                if (flags.echo) {
                    if (received_byte == '\r') {
                        fputc('\n', &out);
                    }
                    fputc(received_byte, &out);
                }
                if (received_byte == '\n' || received_byte == '\r') {
                    command_buffer[command_buffer_index] = '\0';
                    command_buffer_index = 0;
                    parse_command(command_buffer);
                    fprintf(&out, "> ");
                } else {
                    command_buffer[command_buffer_index++] = received_byte;
                }
            } else {
                handle_command_char(& out, static_cast<char>(received_byte));
            }
        }

        CDC_Device_USBTask(&cdc);
        MIDI_Device_USBTask(&usb_midi);
        USB_USBTask();
    }
}

//==============================================================================
// Interrupt Handlers
//==============================================================================

ISR(TIMER1_OVF_vect) {
    static uint8_t servo_pause_counter = 0;
    switch (servo_pause_counter) {
        case 0:
            TCCR1A |= _BV(COM1A0) | _BV(COM1B0) | _BV(COM1C0);
            TCCR1C |= _BV(FOC1A)  | _BV(FOC1B)  | _BV(FOC1C);
            break;
        case 1:
            TCCR1A &= ~(_BV(COM1A0) | _BV(COM1B0) | _BV(COM1C0));
            break;
        case 2:
            OCR1A = one_milli_second + x_position;
            OCR1B = one_milli_second + y_position;
            OCR1C = one_milli_second + angle;
            break;

    }
    servo_pause_counter = (servo_pause_counter + 1) % 5;
}

ISR(TIMER0_COMPA_vect) {
    static uint8_t gui_timeout = 100;
    if (dtr_is_set(&cdc)) {
        if (! flags.menu_mode && ! flags.cli_mode && --gui_timeout == 0) {
            flags.enter_menu_mode = true;
        }
    } else {
        gui_timeout = 100;
        flags.enter_menu_mode = false;
        flags.menu_mode = false;
        flags.cli_mode = false;
    }
}

ISR(USART1_RX_vect/*, ISR_BLOCK*/) {
}

//==============================================================================
// USB Device Handling
//==============================================================================
void EVENT_USB_Device_Connect(void) {
}

void EVENT_USB_Device_Disconnect(void) {
}

void EVENT_USB_Device_ConfigurationChanged(void) {
    bool success = true;

    success &= CDC_Device_ConfigureEndpoints(&cdc);
    success &= MIDI_Device_ConfigureEndpoints(&usb_midi);
}

void EVENT_USB_Device_ControlRequest(void) {
    CDC_Device_ProcessControlRequest(&cdc);
    MIDI_Device_ProcessControlRequest(&usb_midi);

}

void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const cdc_interface_info) {
}

//==============================================================================
// Bootloader
//==============================================================================

void bootloader_check_jump(void) ATTR_INIT_SECTION(3); // run early after reset

uint32_t bootloader_key ATTR_NO_INIT; // keep value across resets

void
bootloader_check_jump(void) { // run pre-main, after reset
    // If the reset source was the watchdog and the key is correct, clear it and jump to the bootloader
    if ((MCUSR & (1 << WDRF)) && (bootloader_key == bootloader_magic_key)) {
        bootloader_key = 0;
        ((void (*)(void))bootloader_address)(); // call bootloader base address
    }
}

void
bootloader_start() {
    // If USB is used, detach from the bus and reset it
    USB_Disable();

    // Disable all interrupts
    cli();

    // Wait two seconds for the USB detachment to register on the host
    Delay_MS(2000UL);

    // Set the bootloader key to the magic value and force a watchdog reset
    bootloader_key = bootloader_magic_key;
    wdt_enable(WDTO_250MS);
    while (true);
}

