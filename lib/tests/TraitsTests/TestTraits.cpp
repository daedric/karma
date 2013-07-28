#include <cassert>
#include <iostream>
#include <utility>
#include "klib/Traits.hpp"

using namespace KLib;


template <typename T>
bool test_is_numeric(T, typename Traits::enable_if<Traits::is_integer<T>>::type* = nullptr)
{
    return true;
}

bool test_is_numeric(...)
{
    return false;
}

int main(int, char**)
{
    static_assert(Traits::is_signed<int>::value, "");
    static_assert(Traits::is_signed<unsigned int>::value == false, "");
    return (test_is_numeric(1234) == true) ? EXIT_SUCCESS : EXIT_FAILURE;
}
