#ifndef UTILS_HPP_
#define UTILS_HPP_

# include "Types.h"
# include "Traits.hpp"

namespace KLib
{

template <typename T, size_t SIZE>
constexpr size_t getArraySize(T (&) [SIZE])
{
    return SIZE;
}

template <typename T>
constexpr size_t getSizeInBits(T)
{
    return sizeof(T) * 8;
}


template <typename NumericType>
constexpr size_t sliceByte(NumericType value, size_t start, size_t end)
{
    return (value >> start) & ((1 << (end - start)) - 1);
}

template<typename Type>
constexpr typename Traits::remove_reference<Type>::type&& move(Type&& t) noexcept
{   return static_cast<typename Traits::remove_reference<Type>::type&&>(t);}

template <typename T>
constexpr T&& forward(typename Traits::remove_reference<T>::type& v) noexcept
{ return static_cast<T&&>(v); }


template<typename T>
constexpr T&& forward(typename Traits::remove_reference<T>::type&& v) noexcept
{
  static_assert(!Traits::is_lvalue_reference<T>::value, "template argument substituting T is an lvalue reference type");
  return static_cast<T&&>(v);
}

template <typename T>
void swap(T t1, T t2)
{
    T dummy = move(t1);
    t1 = move(t2);
    t2 = move(dummy);
}

template <typename T, typename dummy = typename Traits::enable_if<Traits::is_integer<T>>::type>
struct Numeric
{
    static constexpr T max()
    {
        return ~T(0);
    }
};


}

#endif /* UTILS_HPP_ */
