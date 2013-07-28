/*
 * Dumper.hpp
 *
 *  Created on: May 5, 2013
 *      Author: daedric
 */

#ifndef DUMPER_HPP_
# define DUMPER_HPP_

# include "Logger.hpp"
# include "klib/Traits.hpp"

namespace KLib
{

template <typename T, typename Enabled = void>
class Dumper
{
public:

    Dumper(T)
    {}

    void dump(Logger& logger) const
    {
        logger.write("Unable to dump data");
    }
};

template <>
class Dumper<char>
{
public:
    Dumper(char c)
    : c_(c)
    {}

    void dump(Logger& logger) const
    {
        logger.write(c_);
    }

private:
    char c_;
};

template <typename T>
class Dumper<T, typename Traits::enable_if<Traits::is_integer<T>>::type>
{
public:
    Dumper(T i, int base = 0, bool prefix=true)
    : i_(i)
    , base_(base)
    , prefix_(prefix)
    {
    }

    void dump(Logger& logger) const
    {
        unsigned int actual_base = base_;

        if (base_ < 1 || base_ >= kstrlen(base_char))
        {
            actual_base = logger.getDefaultNumberBase();
        }

        const char* prefix = logger.getBasePrefix(actual_base);
        print_integer(logger, actual_base, i_, prefix);
    }

private:

    static constexpr const char* base_char = "0123456789abcdefghijklnmopqrstuvwxyz";

    template <typename TT>
    static void print_integer(Logger& logger, int base, TT integer, const char* prefix = nullptr, typename Traits::disable_if<Traits::is_signed<TT>>::type* = nullptr)
    {
        if (prefix)
        {
            logger.write(prefix);
        }

        if (integer >= base)
        {
            print_integer(logger, base, integer / base);
        }

        logger.write(base_char[integer % base]);
    }

    template <typename TT>
    static void print_integer(Logger& logger, int base, TT integer, const char* prefix = nullptr, typename Traits::enable_if<Traits::is_signed<TT>>::type* = nullptr)
    {
        if (integer < 0)
        {
            logger.write('-');
        }

        if (prefix)
        {
            logger.write(prefix);
        }

        TT tmp = integer / base;

        if (tmp < 0)
        {
            if (integer <= -base)
            {
                print_integer(logger, base, typename Traits::to_unsigned<TT>::type(-tmp));
            }
            logger.write(base_char[-(integer % base)]);
        }
        else
        {
            if (integer >= base)
            {
                print_integer(logger, base, typename Traits::to_unsigned<TT>::type(tmp));
            }
            logger.write(base_char[(integer % base)]);
        }
    }

    T i_;
    int base_;
    bool prefix_;
};

template <typename T>
class Dumper<T*> : public Dumper<size_t>
{
public:
    Dumper(T* ptr)
    : Dumper<size_t>(size_t(ptr), 16, true)
    {}

    void dump(Logger& logger) const
    {
        Dumper<size_t>::dump(logger);
    }
};


}



#endif /* DUMPER_HPP_ */
