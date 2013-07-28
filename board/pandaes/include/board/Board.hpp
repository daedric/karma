/*
 * Board.hpp
 *
 *  Created on: Sep 23, 2012
 *      Author: daedric
 */

#ifndef __BOARD_HPP__
# define __BOARD_HPP__

# include "klib/NonCopyable.hpp"
# include "klib/Traits.hpp"
# include "klib/Logger.hpp"

# include "cpu/CPU.hpp"

# include "Watchdog.hpp"
# include "UART.hpp"

namespace Board
{
    class PandaES : private KLib::NonCopyable
    {
    public:
        PandaES();
        ~PandaES();

        void init();

        static constexpr size_t BOARD_CLOCK             = 48000000;
        static constexpr size_t SERIAL_BAUD_RATE        = 115200;
        static constexpr size_t UART3_ADDR              = 0x48020000;
        static constexpr size_t WATCHDOG_ADDR           = 0x4A314000;

        static constexpr size_t SYSCTRL_GENERAL_CORE    = 0x4A002000;
        static constexpr size_t SYSCTRL_GENERAL_WKUP    = 0x4A30C000;
        static constexpr size_t SYSCTRL_PADCONF_CORE    = 0x4A100000;
        static constexpr size_t SYSCTRL_PADCONF_WKUP    = 0x4A31E000;

        CPU::CurrentCPU&        getCPU()                    { return current_cpu_; }
        CPU::CurrentCPU const&  getCPU()            const   { return current_cpu_;}
        KLib::Logger&           getDefaultLogger()          { return logger_; }

        void dump(KLib::Logger& logger) const
        {
            logger << "Board: PandaEs";
        }

    private:
        UART                uart3_;
        Watchdog            watchdog_;
        KLib::Logger        logger_;
        CPU::CurrentCPU     current_cpu_;
    };


    typedef PandaES CurrentBoard;
}


#endif /* __BOARD_HPP__ */
