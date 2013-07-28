#ifndef WATCHDOG_HPP_
# define WATCHDOG_HPP_

# include "klib/Address.hpp"
# include "klib/Register.hpp"

namespace Board
{
    struct Watchdog
    {
        Watchdog(KLib::Address<4, true, true, true> const & base);
        void disable();
        void enable();

    private:

        void wait();

        KLib::Volatile32bRegister<KLib::RegisterPart<0, 0, 32>> wwps_;
        KLib::Volatile32bRegister<KLib::RegisterPart<0, 0, 32>> wspr_;
    };
}

#endif /* WATCHDOG_HPP_ */
