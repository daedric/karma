#ifndef __CXXABI_HPP__
# define __CXXABI_HPP__

# include "klib/Types.h"

extern "C"
{
int __aeabi_atexit(void (*destructor) (void *), void *arg, void *dso);
void __cxa_finalize(void *f);
}

__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire (__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual();
extern "C" void* memset(void *s, int c, size_t n);

void __call_constructors();

#endif
