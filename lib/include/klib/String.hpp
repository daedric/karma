#ifndef STRING_HPP_
# define STRING_HPP_

# include "Types.h"

namespace KLib
{
namespace detail
{
static inline size_t strlen(const char* str)
{
    int n = 0;
    while (*str++) n++;
    return n;
}
}

# define kstrlen(str) (__builtin_constant_p(str) ? __builtin_strlen(str) : KLib::detail::strlen(str))

}

#endif /* STRING_HPP_ */
