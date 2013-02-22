#ifndef USART2USB_INCLUDED
#define USART2USB_INCLUDED

#include "hardware_config.hpp"

#include "ringbuffer.hpp"

#define USE_FLUSH_TIMER
#define USE_VISCA_SMART_FLUSH

#define SERIAL_2X_UBBRVAL(baud) ((((F_CPU / 8) + (baud / 2)) / (baud)) - 1)

template <typename Config>
class usart2usb {
    private:
        struct usart_to_usb_config {
            enum { size = Config::queue_size };
            typedef char element_type;
        };
        struct usb_to_usart_config {
            enum { size = Config::queue_size };
            typedef char element_type;
        };
        enum {
            flush_timeout = (main_timer::frequency/Config::flush_frequency)
        };
    
        typedef ringbuffer<usart_to_usb_config> usart_to_usb;
        typedef ringbuffer<usb_to_usart_config> usb_to_usart;
        typedef typename Config::status_led              status_led;
    public:
        static inline
        void
        init(USB_ClassInfo_CDC_Device_t * cdc) {
            usart_to_usb::init();
            usb_to_usart::init();
            cdc_device = cdc;
            status_led::make_output();
        }

        static inline
        void
        on_tick() {
# ifdef USE_FLUSH_TIMER
            if (flush_timer++ == flush_timeout) {
                flush_timer = 0;
                flush_flag = true;
            }
# endif
        }

        static inline
        void
        on_usart_receive() {
            uint8_t received_byte = UDR1;
            if (USB_DeviceState == DEVICE_STATE_Configured) { // XXX better use DTR here?
                usart_to_usb::insert(received_byte);
            }
        }

        static inline
        void
        process_queues() {
            if ( ! usb_to_usart::is_full()) {
                int16_t  received_byte = CDC_Device_ReceiveByte(cdc_device);
                if (received_byte >= 0) {
                    usb_to_usart::insert(received_byte);
                }
            }

            uint16_t current_buffer_size = usart_to_usb::count();
            if (flush_flag || current_buffer_size > Config::high_water_mark) {
                flush_flag = false;
                while (current_buffer_size--) {
                    if (CDC_Device_SendByte(cdc_device, usart_to_usb::peek()) != ENDPOINT_READYWAIT_NoError) {
                        break;
                    }
                    usart_to_usb::remove();
                }
            }

            if ( ! usb_to_usart::is_empty()) {
                send_byte(usb_to_usart::remove());
            }
            
        }
        
        static inline
        void
        on_line_encoding_changed(USB_ClassInfo_CDC_Device_t * changed_device) {
            if (changed_device == cdc_device) {
                uint8_t config_mask = 0;
                USB_ClassInfo_CDC_Device_t * cdc = cdc_device;
                switch (cdc->State.LineEncoding.ParityType) {
                    case CDC_PARITY_Odd:
                        config_mask = _BV(UPM11) | _BV(UPM10);
                        break;
                    case CDC_PARITY_Even:
                        config_mask = _BV(UPM11);
                        break;
                }

                if (cdc->State.LineEncoding.CharFormat == CDC_LINEENCODING_TwoStopBits) {
                    config_mask |= _BV(USBS1);
                }

                switch (cdc->State.LineEncoding.DataBits) {
                    case 6:
                        config_mask |= _BV(UCSZ10);
                        break;
                    case 7:
                        config_mask |= _BV(UCSZ11);
                        break;
                    case 8:
                        config_mask |= _BV(UCSZ10) | _BV(UCSZ11);
                        break;
                }

                UCSR1B = 0;
                UCSR1A = 0;
                UCSR1C = 0;

                UBRR1 = SERIAL_2X_UBBRVAL(cdc->State.LineEncoding.BaudRateBPS);

                UCSR1C = config_mask;
                UCSR1A = _BV(U2X1);
                UCSR1B = _BV(RXCIE1) | _BV(TXEN1) | _BV(RXEN1);
            }
        }
    private:

        static inline
        void
        send_byte(const char byte) {
            while ( ! (UCSR1A & _BV(UDRE1)));
            UDR1 = byte;
        }
        static volatile bool flush_flag;
        static USB_ClassInfo_CDC_Device_t * cdc_device;
        static uint16_t flush_timer;
};

template <typename Config>
volatile bool usart2usb<Config>::flush_flag = false;

template <typename Config>
uint16_t usart2usb<Config>::flush_timer = 0;

template <typename Config>
USB_ClassInfo_CDC_Device_t * usart2usb<Config>::cdc_device = NULL;

#endif // USART2USB_INCLUDED
