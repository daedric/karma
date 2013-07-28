#include "klib/Types.h"
#include "klib/Utils.hpp"
#include "klib/Logger.hpp"
#include "klib/Register.hpp"

#include "board/Board.hpp"
#include "board/UART.hpp"
#include "kern/CXXABI.hpp"

extern "C" void kmain()
{

    __call_constructors();

    Board::CurrentBoard board;
    auto& logger = board.getDefaultLogger();

    int i = 0;
    logger << dump_data(&i) << KLib::endl;

    logger << "this is a log from kernel :D" << KLib::endl;
    logger << "this does work" << KLib::endl;
    logger << "Board print" << board << 123 << KLib::endl;

    logger << "hello world" << KLib::endl;
    logger << 123 << KLib::endl;
    logger << board << KLib::endl;

    board.init();

    logger << "hello world2" << KLib::endl;

    while (1)
    {
    }
}
