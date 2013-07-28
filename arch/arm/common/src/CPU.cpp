#include "common/CPU.hpp"

using namespace ARM;

CPU::CPU(KLib::Logger& logger)
: logger_(logger)
{}

CPU::~CPU(){}


bool CPU::disableInterrupts()
{
    unsigned long old,temp;
    __asm__ __volatile__("mrs %0, cpsr\n"
                 "orr %1, %0, #0xc0\n"
                 "msr cpsr_c, %1"
                 : "=r" (old), "=r" (temp)
                 :
                 : "memory");
    return (old & 0x80) == 0;
}

void CPU::enableInterrupts()
{
    unsigned long temp;
    __asm__ __volatile__("mrs %0, cpsr\n"
                 "bic %0, %0, #0x80\n"
                 "msr cpsr_c, %0"
                 : "=r" (temp)
                 :
                 : "memory");
}
