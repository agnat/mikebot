#ifndef PUSHBUTTONS_INCLUDED
#define PUSHBUTTONS_INCLUDED

template <typename Config>
class pushbuttons {
    private:
        enum {
            poll_interval = main_timer::frequency / Config::debounce_poll_frequency
        };
        typedef typename Config::port port;

    public:
        static inline
        void
        init() {
            port::enable_pullups();
            for (uint8_t i = 0; i < Config::debounce_sample_count; ++i) {
                history_[i] = 0;
            }
        }
        static inline
        void
        on_tick() {
            if (timer_++ == poll_interval) {
                timer_ = 0;

                //store the current button values in the history ringbuffer
                history_[history_index_++] = port::read();
                history_index_ %= Config::debounce_sample_count;

                uint8_t buttons_up = 0xff;
                uint8_t buttons_down = 0xff;
                for (uint8_t i = 0; i < Config::debounce_sample_count; ++i) {
                    buttons_up &= history_[i];
                    buttons_down &= ~history_[i];
                }
                buttons_up_ = buttons_up;
                buttons_down_ = buttons_down;
            }
        }
        static inline
        void
        report_events(FILE * stream) {
            uint8_t current_up;
            uint8_t current_down;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                current_up = buttons_up_;
                current_down = buttons_down_;
            }
            if ((reported_up_ != current_up || reported_down_ != current_down)) {
                if (stream) {
                    for (uint8_t i = 0; i < Config::num_buttons; ++i) {
                        uint8_t mask = _BV(i);
                        if ( ! (mask & reported_up_) && (mask & current_up)) {
                            fprintf(stream, "%s 0\r\n", button_names_[i]);
                        }
                        if ( ! (mask & reported_down_) && (mask & current_down)) {
                            fprintf(stream, "%s 1\r\n", button_names_[i]);
                        }
                    }
                }
                reported_up_ = current_up;
                reported_down_ = current_down;
            }
        }
        static inline
        void
        report_state(FILE * stream) {
            for (uint8_t i = 0; i < Config::num_buttons; ++i) {
                uint8_t mask = _BV(i);
                if (mask & reported_up_) {
                    fprintf(stream, "%s 0\r\n", button_names_[i]);
                }
                if (mask & reported_down_) {
                    fprintf(stream, "%s 1\r\n", button_names_[i]);
                }
            }
        }

    private:
        static uint16_t          timer_;
        static uint8_t           history_[Config::debounce_sample_count];
        static uint8_t           history_index_;
        static volatile uint8_t  buttons_up_;
        static volatile uint8_t  buttons_down_;
        static uint8_t           reported_up_;
        static uint8_t           reported_down_;
        static const char * const button_names_[];
};

template <typename Config>
uint16_t pushbuttons<Config>::timer_ = 0;

template <typename Config>
uint8_t pushbuttons<Config>::history_[Config::debounce_sample_count];

template <typename Config>
uint8_t pushbuttons<Config>::history_index_ = 0;

template <typename Config>
volatile uint8_t pushbuttons<Config>::buttons_up_ = 0;

template <typename Config>
volatile uint8_t pushbuttons<Config>::buttons_down_ = 0;

template <typename Config>
uint8_t pushbuttons<Config>::reported_down_ = 0;

template <typename Config>
uint8_t pushbuttons<Config>::reported_up_ = 0;

template <typename Config>
const char * const pushbuttons<Config>::button_names_[] = {"a", "q", "l", "p"};

#endif // PUSHBUTTONS_INCLUDED
