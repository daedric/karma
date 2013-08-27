#include "cpu/Memory.hpp"
#include "klib/Address.hpp"
#include "klib/Register.hpp"
#include "board/Board.hpp"

using namespace Board;

static PandaES::PageAllocator __section__("data") page_allocator_;

PandaES::PandaES()
: uart3_(BOARD_CLOCK, UART3_ADDR)
, watchdog_(WATCHDOG_ADDR)
, logger_(&uart3_)
, current_cpu_(logger_)
, mmu_(logger_)
{
}

PandaES::~PandaES()
{
}

void PandaES::init()
{
//	uart3_.init(PandaES::SERIAL_BAUD_RATE);

	current_cpu_.setHiVec();
	size_t vbase = current_cpu_.getInterruptVectorAddress();
	this->getDefaultLogger() << "Address to load the interrupt vector: " << vbase << KLib::endl;

    mmu_.prepareMandatoryPages(page_allocator_);
    mmu_.init();

	current_cpu_.setupInterruptHandlers();


	this->getDefaultLogger() << "Board initialized" << KLib::endl;
}

