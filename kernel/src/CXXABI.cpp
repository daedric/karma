#include "klib/Utils.hpp"
#include "kern/CXXABI.hpp"

struct atexit_entry
{
    void (*destructor)(void*);
    void *obj_inst, *dso_handle;
} entries[512];

static unsigned int nb_entries;

extern "C"
{
    void *__dso_handle = nullptr;
}
//
//// from section 7.3.1 Unsigned Division by Trial Subtraction of
//// ARM System Developer's Guide
//extern "C" unsigned int __aeabi_uidiv(unsigned int n, unsigned int d)
//{
//  unsigned int N = sizeof(int) * 8;
//  unsigned q = 0, r = n;
//  do
//  {
//      N--;
//      if ((r >> N) >= d)
//      {
//          r -= (d << N);
//          q += (1 << N);
//      }
//  } while (N);
//  return q;
//}

extern "C"
int raise(int)
{
    return 0;
}

extern "C" int __aeabi_atexit(void (*f)(void *), void *objptr, void *dso)
{
    if (nb_entries >= KLib::getArraySize(entries))
    {
        return -1;
    }
    entries[nb_entries].destructor = f;
    entries[nb_entries].obj_inst = objptr;
    entries[nb_entries].dso_handle = dso;
    ++nb_entries;
    return 0;
}

extern "C" void strcpy(char* str, const char* from)
{
    while (*from)
    {
        *str++ = *from++;
    }
    *str = 0;
}

extern "C" void __eabi_finalize(void *f)
{
    int i = nb_entries;
    if (!f)
    {
        while (i--)
        {
            if (entries[i].destructor)
            {
                (*entries[i].destructor)(entries[i].obj_inst);
            };
        };
        return;
    };

    for (; i >= 0; --i)
    {
        if (entries[i].destructor == f)
        {
            (*entries[i].destructor)(entries[i].obj_inst);
            entries[i].destructor = nullptr;
        }
    }
}

extern "C" int __cxa_guard_acquire(__guard *g)
{
    return !*(char *) (g);
}

extern "C" void __cxa_guard_release(__guard *g)
{
    *(char *) g = 1;
}

extern "C" void __cxa_guard_abort(__guard *)
{

}

extern "C" void __cxa_pure_virtual()
{

}

extern "C" void* memset(void *s, int c, size_t n)
{
    for (int* mem = (int*)s; n; n--)
    {
        *mem++ = c;
    }
    return s;
}

typedef void (*CTOR)();
extern "C"
{
    extern void* start_ctors;
    extern void* end_ctors;
}

void __call_constructors()
{
    size_t nb_ctors = (size_t(&end_ctors) - size_t(&start_ctors))
            / sizeof(void*);

    CTOR* addr = reinterpret_cast<CTOR*>(&start_ctors);

    for (size_t i = 0; i < nb_ctors; ++i, ++addr)
    {
        (*addr)();
    }
}
