#include "klib/Register.hpp"

#include "cpu/CPU.hpp"

using namespace CPU;


ARMV7a::ARMV7a(KLib::Logger& logger)
: ARM::CPU(logger)
{}

ARMV7a::~ARMV7a()
{}

extern "C"
{
extern void* __interrupt_vtable__;
extern void* __end_interrupt_vtable__;
}


void ARMV7a::setupInterruptHandlers()
{
}
