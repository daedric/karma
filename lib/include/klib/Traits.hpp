#ifndef TRAITS_HPP_
# define TRAITS_HPP_

namespace KLib { namespace Traits {

struct true_type
{
    static const bool value = true;
};

struct false_type
{
    static const bool value = false;
};

template <bool Cond, typename True, typename False>
struct static_if_c
{
    typedef False type;
};

template <typename True, typename False>
struct static_if_c<true, True, False>
{
    typedef True type;
};

template <typename C, typename True, typename False>
struct static_if : public static_if_c<C::value, True, False> {};

template <typename T, typename U>
struct is_same : false_type
{};

template <typename T>
struct is_same<T, T> : true_type
{};

template <bool Cond, typename = void>
struct enable_if_c
{

};

template <typename T>
struct enable_if_c<true, T>
{
    typedef T type;
};

template <typename T, typename = void>
struct enable_if : enable_if_c<T::value, void> {};


template <typename T, typename = void>
struct disable_if : enable_if_c<!T::value, void> {};


template <typename T> struct is_pointer : false_type {};
template <typename T> struct is_pointer<T*> : true_type{};


template <typename T> struct is_reference : false_type {};
template <typename T> struct is_reference<T&> : true_type {};

template <typename T> struct is_const : false_type {};
template <typename T> struct is_const<T const> : true_type {};

template <typename T> struct is_volatile : false_type {};
template <typename T> struct is_volatile<T volatile> : true_type {};

template <typename T>
struct add_pointer
{
    typedef T* type;
};

template <typename T>
struct add_ref
{
    typedef T& type;
};

template <typename T>
struct add_const
{
    typedef T const type;
};

template <typename T>
struct add_volatile
{
    typedef T volatile type;
};

template <typename T>
struct remove_pointer
{
    typedef T type;
};

template <typename T>
struct remove_pointer<T*>
{
    typedef typename remove_pointer<T>::type type;
};

template <typename T>
struct remove_reference
{
    typedef T type;
};

template <typename T>
struct remove_reference<T&>
{
    typedef T type;
};

template <typename T>
struct remove_reference<T&&>
{
    typedef T type;
};


template <typename T> struct is_lvalue_reference : false_type {};
template <typename T> struct is_lvalue_reference<T&> : true_type {};

template <typename T> struct is_integer : false_type {};

template <> struct is_integer<int> : true_type {};
template <> struct is_integer<char> : true_type {};
template <> struct is_integer<long> : true_type {};
template <> struct is_integer<short> : true_type {};
template <> struct is_integer<unsigned int> : true_type {};
template <> struct is_integer<unsigned char> : true_type {};
template <> struct is_integer<unsigned long> : true_type {};
template <> struct is_integer<unsigned short> : true_type {};


template <typename T> struct is_signed;

template <> struct is_signed<int> : true_type {};
template <> struct is_signed<char> : true_type {};
template <> struct is_signed<long> : true_type {};
template <> struct is_signed<short> : true_type {};
template <> struct is_signed<unsigned int> : false_type {};
template <> struct is_signed<unsigned char> : false_type {};
template <> struct is_signed<unsigned long> : false_type {};
template <> struct is_signed<unsigned short> : false_type {};

template <typename T> struct to_unsigned;

template <> struct to_unsigned<int>   { typedef unsigned int   type; };
template <> struct to_unsigned<char>  { typedef unsigned char  type; };
template <> struct to_unsigned<long>  { typedef unsigned long  type; };
template <> struct to_unsigned<short> { typedef unsigned short type; };

template <typename Logger, typename Object>
struct has_print_function
{
    using yes = struct { char _; };
    using no = struct { char _[2]; };


    template <typename T, void (T::*print_function)(Logger&) const>
    struct Dummy
    {
    };

    template <typename T>
    static yes _is_printable(T*, Dummy<T, &T::print>* = nullptr);

    template <typename>
    static no _is_printable(...);

    static const bool value =  sizeof(_is_printable<Object>((Object*)nullptr)) == sizeof(yes);

};

}}

#endif /* TRAITS_HPP_ */
