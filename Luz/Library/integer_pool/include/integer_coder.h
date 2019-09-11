#pragma once
#ifndef ppg_integer_coder_h
#define ppg_integer_coder_h

#include <stdint.h>

namespace ppg
{
    template<typename integer_type>
    class integer_coder
    {
        static constexpr unsigned int sm_num_integer_bits = sizeof(integer_type) << 3;

    public:

        template<typename uint_type, integer_type integer_range, unsigned int num_code_bits>
        static inline integer_type encode(const integer_type i, const uint_type code)
        {
            static_assert(num_code_bits < sm_num_integer_bits);
            static_assert((1U << (sm_num_integer_bits - num_code_bits)) < integer_range);

            return i | static_cast<integer_type>(code << (sm_num_integer_bits - num_code_bits));
        }

        template<typename uint_type, integer_type integer_range>
        static inline integer_type encode(const integer_type i, const uint_type code, const unsigned int num_code_bits)
        {
            return i | static_cast<integer_type>(code << (sm_num_integer_bits - num_code_bits));
        }

        template<typename uint_type, integer_type integer_range, unsigned int num_code_bits>
        static inline integer_type decode(const integer_type i, uint_type* p_code)
        {
            static constexpr s_num_uint_bits = sizeof(uint_type) << 3;
            static_assert(num_code_bits < s_num_uint_bits);

            if (p_code)
            {
                integer_type mask = ~(static_cast<integer_type>(~0U) << num_code_bits);
                integer_type shift = static_cast<integer_type>(i) >> (sm_num_integer_bits - num_code_bits);
                *p_code = static_cast<uint_type>(shift & mask);
            }

            return i & (integer_range - 1);
        }

        template<typename uint_type, integer_type integer_range>
        static inline integer_type decode(const integer_type i, const unsigned int num_code_bits, uint_type* p_code)
        {
            if (p_code)
            {
                integer_type mask = ~(static_cast<integer_type>(~0U) << num_code_bits);
                integer_type shift = static_cast<integer_type>(i) >> (sm_num_integer_bits - num_code_bits);
                *p_code = static_cast<uint_type>(shift & mask);
            }

            return i & (integer_range - 1);
        }

        template<typename uint_type, unsigned int num_code_bits>
        static inline uint_type extract(const integer_type i)
        {
            static constexpr s_num_uint_bits = sizeof(uint_type) << 3;
            static_assert(num_code_bits < s_num_uint_bits);
            
            integer_type mask = ~(static_cast<integer_type>(~0U) << num_code_bits);
            integer_type shift = static_cast<integer_type>(i) >> (sm_num_integer_bits - num_code_bits);
            *p_code = static_cast<uint_type>(shift & mask);
        }

        template<typename uint_type>
        static inline uint_type extract(const integer_type i, const unsigned int num_code_bits)
        {
            integer_type mask = ~(static_cast<integer_type>(~0U) << num_code_bits);
            integer_type shift = static_cast<integer_type>(i) >> (sm_num_integer_bits - num_code_bits);
            return static_cast<uint_type>(shift & mask);
        }
    };
}

#endif