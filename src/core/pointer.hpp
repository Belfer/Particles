#pragma once

#include <SFML/System.hpp>

using uint = unsigned int;

using u8 = sf::Uint8;
using u16 = sf::Uint16;
using u32 = sf::Uint32;
using u64 = sf::Uint64;

using uptr = uintptr_t;

namespace pointer
{
    inline void* alignForward(void* address, u8 alignment)
    {
        return (void*)( ( reinterpret_cast<uptr>(address) + static_cast<uptr>(alignment-1) ) & static_cast<uptr>(~(alignment-1)) );
    }

    inline const void* alignForward(const void* address, u8 alignment)
    {
        return (void*)( ( reinterpret_cast<uptr>(address) + static_cast<uptr>(alignment-1) ) & static_cast<uptr>(~(alignment-1)) );
    }

    inline void* alignBackward(void* address, u8 alignment)
    {
        return (void*)( reinterpret_cast<uptr>(address) & static_cast<uptr>(~(alignment-1)) );
    }

    inline const void* alignBackward(const void* address, u8 alignment)
    {
        return (void*)( reinterpret_cast<uptr>(address) & static_cast<uptr>(~(alignment-1)) );
    }

    inline u8 alignForwardAdjustment(const void* address, u8 alignment)
    {
        u8 adjustment =  alignment - ( reinterpret_cast<uptr>(address) & static_cast<uptr>(alignment-1) );

        if(adjustment == alignment)
                return 0; //already aligned

        return adjustment;
    }

    inline u8 alignForwardAdjustmentWithHeader(const void* address, u8 alignment, u8 headerSize)
    {
        u8 adjustment =  alignForwardAdjustment(address, alignment);

        u8 neededSpace = headerSize;

        if(adjustment < neededSpace)
        {
            neededSpace -= adjustment;

            //Increase adjustment to fit header
            adjustment += alignment * (neededSpace / alignment);

            if(neededSpace % alignment > 0)
                adjustment += alignment;
        }

        return adjustment;
    }

    inline u8 alignBackwardAdjustment(const void* address, u8 alignment)
    {
        u8 adjustment =  reinterpret_cast<uptr>(address) & static_cast<uptr>(alignment-1);

        if(adjustment == alignment)
            return 0; //already aligned

        return adjustment;
    }

    inline void* add(void* p, size_t x)
    {
        return (void*)( reinterpret_cast<uptr>(p) + x);
    }

    inline const void* add(const void* p, size_t x)
    {
        return (const void*)( reinterpret_cast<uptr>(p) + x);
    }

    inline void* subtract(void* p, size_t x)
    {
        return (void*)( reinterpret_cast<uptr>(p) - x);
    }

    inline const void* subtract(const void* p, size_t x)
    {
        return (const void*)( reinterpret_cast<uptr>(p) - x);
    }
}
