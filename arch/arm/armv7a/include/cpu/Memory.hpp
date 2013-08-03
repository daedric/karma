#ifndef MEMORY_HPP_
#define MEMORY_HPP_

# include "klib/Types.h"
# include "klib/Address.hpp"
# include "klib/Register.hpp"
# include "klib/Logger.hpp"
# include "klib/compiler/compiler.hpp"
# include "common/Memory.hpp"
# include "MemoryRegister.hpp"

namespace CPU
{

class MMUHelper
{
};

class MMU
{
public:
    MMU(){}
    ~MMU(){}

    static inline void flushTLB()
    {
        unsigned int zero = 0;
        asm volatile ("MCR p15, 0, [zero], c8, c7, 0":: [zero] "r"(zero) );
    }

    static void* get_kernel_end();

    using LargePage = CPU::LargePage;
    using SmallPage = CPU::SmallPage;
    using MediumPage = CPU::MediumPage;

    static bool AllocatePage(LargePage&, size_t, size_t pa, size_t va);
    static bool AllocatePage(SmallPage& page, size_t base_pa, size_t pa, size_t va);
    static bool AllocatePage(MediumPage& page, size_t base_pa, size_t pa, size_t va);

    template <typename PageAllocator>
    void prepareMandatoryPage(PageAllocator& alloc)
    {
        // Assume identity mapping for kernel and devices
        void* mem = nullptr;
        size_t size = 0;

        {
            auto const kern_end = size_t(get_kernel_end());
            do
            {
                alloc.allocateKernelPage(mem, size);
            } while (size_t(mem) + size < kern_end);
        }

        {
            while (alloc.allocateDevicePage(mem, size));
        }
    }

    void init();
    void disable();

private:
    static Directory l1;
}
;
}

#endif /* MEMORY_HPP_ */
