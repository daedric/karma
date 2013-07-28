#ifndef LOGGERUTILS_HPP_
# define LOGGERUTILS_HPP_

namespace KLib
{
class Logger;

namespace detail
{
typedef struct { int _[665]; } yes;
typedef struct { int _[666]; } no;

template<typename Dumpable>
struct has_dump_method
{
    template<typename T, void (T::*)(Logger&) const>
    struct __dummy__
    {
    };

    template<typename T>
    static yes has_dump(__dummy__ <T, &T::dump>*);

    template<typename >
    static no has_dump(...);

    static const bool value = (sizeof(has_dump<Dumpable>(0)) == sizeof(yes));

};


template<typename Stream, typename U>
static no operator<<(Stream&, U const&);

template<typename Stream, typename Object>
struct HasDoubleRedirectionOperator
{
    static const bool value = sizeof(*(Stream*) 0 << *(Object*) 0) != sizeof(no);
};
}
}



#endif /* LOGGERUTILS_HPP_ */
