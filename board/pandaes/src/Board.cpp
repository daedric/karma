#include "cpu/Memory.hpp"
#include "klib/Address.hpp"
#include "klib/Register.hpp"
#include "board/Board.hpp"

using namespace Board;

PandaES::PandaES()
: uart3_(BOARD_CLOCK, UART3_ADDR)
, watchdog_(WATCHDOG_ADDR)
, logger_(&uart3_)
, current_cpu_(logger_)
{
}

PandaES::~PandaES()
{
}

static CPU::MemoryRegion regions[] =
{
 	{0x40000000,		1_Go,		0x40000000, CPU::PageSize::P1M, CPU::MemoryType::Devices},
 	{0x80000000,		1_Mo,		0x80000000, CPU::PageSize::P4k, CPU::MemoryType::Kernel},
 	{0x80000000 + 1_Mo, 990_Mo,		0xC0000000, CPU::PageSize::P4k, CPU::MemoryType::RAM},
};

void PandaES::init()
{
//	uart3_.init(PandaES::SERIAL_BAUD_RATE);

	current_cpu_.setHiVec();
	size_t vbase = current_cpu_.getInterruptVectorAddress();
	this->getDefaultLogger() << "Address to load the interrupt vector: " << vbase << KLib::endl;

	current_cpu_.setupMMU(this->getDefaultLogger(), regions, KLib::getArraySize(regions));
	current_cpu_.setupInterruptHandlers();


	this->getDefaultLogger() << "Board initialized" << KLib::endl;
}

