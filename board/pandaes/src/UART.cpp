#include "klib/Utils.hpp"
#include "board/UART.hpp"

using namespace Board;

UART::UART(size_t clock, UARTBaseAddress address)
: KLib::Driver(KLib::Driver::CAN_READ | KLib::Driver::CAN_WRITE)
, clock_(clock)
, base_address_(address)
, data_	(address)
, ier_	(address + 0x4)
, fcr_	(address + 0x8)
, lcr_	(address + 0xc)
, mcr_	(address + 0x10)
, lsr_	(address + 0x14)
, msr_	(address + 0x18)
, spr_	(address + 0x1C)
, mdr1_	(address + 0x20)
{}

UART::~UART()
{}


void UART::init(size_t baud)
{
	auto baud_rate_divisor = (this->clock_ + (baud * (16 / 2))) /
		(16 * baud);

	ier_.writeValue(0x0);
	mdr1_.setValue<detail::MDR1RegisterPart::MODE_SELECT>(0x7); // reset

	lcr_.setValue<detail::LCRRegiserPart::DIV_EN>(0x1)
			.setValue<detail::LCRRegiserPart::CHAR_LENGTH>(0x3);

	data_.setValue<detail::DataRegisterPart::DLL>(0);
	ier_.setValue<detail::IERRegisterPart::DLH>(0);

	lcr_.setValue<detail::LCRRegiserPart::CHAR_LENGTH>(0x3);

	mcr_.setValue<detail::MCRRegisterPart::DTR>(0x1)
			.setValue<detail::MCRRegisterPart::RTS>(0x1);

	fcr_.setValue<detail::FCRRegisterPart::FIFO_EN>(0x1)
			.setValue<detail::FCRRegisterPart::RX_FIFO_CLEAR>(0x1)
			.setValue<detail::FCRRegisterPart::TX_FIFO_CLEAR>(0x1);

	lcr_.setValue<detail::LCRRegiserPart::DIV_EN>(0x1)
			.setValue<detail::LCRRegiserPart::CHAR_LENGTH>(0x3);

	data_.setValue<detail::DataRegisterPart::DLL>(baud_rate_divisor & 0xff);
	ier_.setValue<detail::IERRegisterPart::DLH>((baud_rate_divisor >> 8) & 0xff);
	lcr_.setValue<detail::LCRRegiserPart::CHAR_LENGTH>(0x3);
	mdr1_.writeValue(0x0);
}
