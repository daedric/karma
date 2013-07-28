#ifndef COMMON_MEMORY_HPP_
#define COMMON_MEMORY_HPP_

namespace ARM
{
    enum class PageSize
    {
        P4k = 4_Ko,
        P64k = 64_Ko,
        P1M = 1_Mo,
    };

    enum class MemoryType
    {
        Devices,
        InterruptHandler,
        Kernel,
        RAM
    };

    struct MemoryRegion
    {
        size_t physical_base_address;
        size_t size;
        size_t virtual_base_address;
        PageSize    page_size;
        MemoryType  memory_type;
    };
}

inline void* operator new(unsigned int size) noexcept;
inline void* operator new(unsigned int, void* ptr) noexcept
{
    return ptr;
}

inline void* operator new[](unsigned int, void*) noexcept;
inline void operator delete(void*) noexcept
{
}
inline void operator delete(void*, void*) noexcept;
inline void operator delete[](void*, void*) noexcept;


#endif /* COMMON_MEMORY_HPP_ */
