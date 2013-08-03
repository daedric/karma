#ifndef COMMON_MEMORY_HPP_
#define COMMON_MEMORY_HPP_

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
