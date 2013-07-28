/*
 * CPU.cpp
 *
 *  Created on: Sep 23, 2012
 *      Author: daedric
 */

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
//
//  this->disableInterrupts();
//
//  size_t nb_interrupt = (size_t(&__end_interrupt_vtable__) - size_t(&__interrupt_vtable__))
//          / sizeof(void*);
//  size_t* it          = reinterpret_cast<size_t*>(size_t(&__interrupt_vtable__));
//  size_t* end         = reinterpret_cast<size_t*>(size_t(&__end_interrupt_vtable__));
//  size_t* destination = reinterpret_cast<size_t*>(getInterruptVectorAddress());
//
//  this->get_logger() << "Number of interrupt handlers: " << nb_interrupt << KLib::endl;
//  this->get_logger() << "The interrupt handle will be installed at: " << size_t(destination) << KLib::endl;
//
//  while (it != end)
//  {
//      this->get_logger() << "Pointer: " << (size_t)it << ", Data: " << *it << KLib::endl;
//      *destination++ = *it++;
//  }
//
//  this->enableInterrupts();
}
