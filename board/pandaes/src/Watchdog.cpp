#include "board/Watchdog.hpp"

using namespace Board;

Watchdog::Watchdog(KLib::Address<4, true, true, true> const & base)
    : wwps_(base + 0x34)
    , wspr_(base + 0x48)
{

}

/*
 * OMAP4460 says:
 * To disable the timer, follow this sequence: 1. Write 0xXXXX AAAA in WDT_WSPR. 2. Write 0xXXXX 5555 in WDT_WSPR.
 * To enable the timer, follow this sequence: 1. Write 0xXXXX BBBB in WDT_WSPR. 2. Write 0xXXXX 4444 in WDT_WSPR.
 */

void Watchdog::disable()
{
    wspr_.writeValue(0xAAAA);
    wait();
    wspr_.writeValue(0x5555);
    wait();
}

void Watchdog::enable()
{
    wspr_.writeValue(0xBBBB);
    wait();
    wspr_.writeValue(0x4444);
    wait();
}

void Watchdog::wait()
{
    int wait = 0;
    do
    {
        wait = wwps_.getValue<0, int>();
    } while (wait);
}


