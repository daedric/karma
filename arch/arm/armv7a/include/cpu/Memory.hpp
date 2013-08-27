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
    MMU(KLib::Logger& log)
    : logger_(log)
    {
        memset(&l1, 0, sizeof(l1));
        logger = &log;
    }
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
    void prepareMandatoryPages(PageAllocator& alloc)
    {
        logger_ << "Prepare the mandatory pages..." << KLib::endl;
        // Assume identity mapping for kernel and devices
        void* mem = nullptr;
        size_t size = 0;

        {
            auto const kern_end = size_t(get_kernel_end());
            do
            {
                logger_ << ".";
                if (!alloc.allocateKernelPage(mem, size))
                {
                    logger_ << "Cannot allocate mandatory kernel page" << KLib::endl;
                }
            } while (size_t(mem) + size < kern_end);

            logger_ << " Kernel pages ready" << KLib::endl;
        }

        {
            logger_ << "Map devices pages...";
            while (alloc.allocateDevicePage(mem, size))
            {
                logger_ << ".";
            }
            logger_ << " Device pages ready" << KLib::endl;
        }

        logger_ << "Kernel mandatory pages allocated !" << KLib::endl;
    }

    void init();
    void disable();

private:
    static Directory l1;
    static KLib::Logger* logger;
    KLib::Logger& logger_;
}
;
}

#endif /* MEMORY_HPP_ */
