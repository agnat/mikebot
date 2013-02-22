#ifndef AVR_PLUS_PLUS_RINGBUFFER_INCLUDED
#define AVR_PLUS_PLUS_RINGBUFFER_INCLUDED

#include "util/atomic.h"

template <typename Config>
class ringbuffer {
    private:
    public:
        enum { size = Config::size };
        typedef typename Config::element_type element_type;

        static inline
        void
        init() {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
                out_ = in_ = start_ = buffer_;
                end_ = &buffer_[size];
                count_ = 0;
            }
        }
        static inline
        uint16_t
        count() {
            uint16_t count;
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
                count = count_;
            }
            return count;
        }
        static inline
        bool
        is_full() {
            return count() == size;
        }
        static inline
        bool
        is_empty() {
            return count() == 0;
        }
        static inline
        void
        insert(element_type data) {
            *in_ = data;
            if (++in_ == end_) {
                in_ = start_;
            }
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
                ++count_;
            }
        }
        static inline
        element_type remove() {
            element_type data = *out_;

            if (++out_ == end_) {
                out_ = start_;
            }

            ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
                --count_;
            }
            return data;
        }
        static inline
        uint16_t
        peek() {
            return *out_;
        }
    private:
        static element_type buffer_[size];
        static element_type * in_;
        static element_type * out_;
        static element_type * start_;
        static element_type * end_;
        static uint16_t       count_;

};

template <typename Config>
typename ringbuffer<Config>::element_type ringbuffer<Config>::buffer_[];

template <typename Config>
typename ringbuffer<Config>::element_type * ringbuffer<Config>::in_ = NULL;

template <typename Config>
typename ringbuffer<Config>::element_type * ringbuffer<Config>::out_ = NULL;

template <typename Config>
typename ringbuffer<Config>::element_type * ringbuffer<Config>::start_ = NULL;

template <typename Config>
typename ringbuffer<Config>::element_type * ringbuffer<Config>::end_ = NULL;

template <typename Config>
uint16_t ringbuffer<Config>::count_ = 0;

#endif // AVR_PLUS_PLUS_RINGBUFFER_INCLUDED
