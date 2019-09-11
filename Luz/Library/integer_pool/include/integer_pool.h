#pragma once
#ifndef ppg_integer_pool_h
#define ppg_integer_pool_h

#include <atomic>

namespace ppg
{
    template<typename integer_type, typename storage_type, unsigned int num_integers>
    class integer_pool
    {
        typedef uint32_t uint;
        typedef storage_type storage_t;
        typedef std::atomic<storage_t> atomic_storage_t;
        static constexpr uint sm_num_storage_bits = sizeof(storage_t) * 8;
        static constexpr uint sm_num_storage = num_integers / sm_num_storage_bits;
        static_assert(num_integers% sm_num_storage == 0, "integer_pool requires num_integers be a multiple of sm_num_storage_bits");
        static_assert(num_integers <= (std::numeric_limits<integer_type>::max)(), "integer_pool requires num_integers be within range of integer_type");

    public:

        static constexpr integer_type failure = 0;

        integer_pool();

        ~integer_pool() = default;

        integer_type allocate();

        void free(const integer_type i);

    private:

        atomic_storage_t m_storage[sm_num_storage];
    };

    template<typename integer_type, typename storage_type, unsigned int num_integers>
    integer_pool<integer_type, storage_type, num_integers>::integer_pool()
    {
        // Use zero to signal allocation failed
        m_storage[0].store(1);

        for (uint i = 1; i < sm_num_storage; ++i)
            m_storage[i].store(ATOMIC_VAR_INIT(0));
    }

    template<typename integer_type, typename storage_type, unsigned int num_integers>
    integer_type integer_pool<integer_type, storage_type, num_integers>::allocate()
    {
        uint i, j;
        storage_t current, updated;
        bool success = false;

        for (i = 0; i < sm_num_storage; ++i)
        {
            current = m_storage[i].load();

            for (j = 0; j < sm_num_storage_bits; j++)
            {
                do
                {
                    success = (current & (1 << j)) == 0;
                    if (!success) break;

                    // Found Free handle.. enter cas loop
                    updated = current | (1 << j);
                } while (!m_storage[i].compare_exchange_strong(current, updated));

                // break inner
                if (success) break;
            }

            // break outer
            if (success) break;
        }

        return (success)
            ? static_cast<integer_type>(i * sm_num_storage_bits + j)
            : failure;
    }

    template<typename integer_type, typename storage_type, unsigned int num_integers>
    void integer_pool<integer_type, storage_type, num_integers>::free(const integer_type i)
    {
        uint s = i / static_cast<uint>(sm_num_storage_bits);
        uint b = i % static_cast<uint>(sm_num_storage_bits);

        storage_t current = m_storage[s].load();
        storage_t updated;

        do
        {
            updated = current & ~(1 << b);
        } while (!m_storage[s].compare_exchange_strong(current, updated));
    }
}

#endif