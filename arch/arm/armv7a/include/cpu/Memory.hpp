/*
 * Memory.hpp
 *
 *  Created on: 11 oct. 2012
 *      Author: Daedric
 */

#ifndef MEMORY_HPP_
#define MEMORY_HPP_

# include "klib/Types.h"
# include "klib/Address.hpp"
# include "klib/Logger.hpp"
# include "common/Memory.hpp"

namespace CPU
{

    using ARM::MemoryRegion;
    using ARM::PageSize;
    using ARM::MemoryType;

    typedef size_t paddr;
    typedef size_t vaddr;

    class MMU
    {
    public:
        MMU();
        ~MMU();

        static inline void flushTLB()
        {
            unsigned int zero = 0;
            asm volatile ("MCR p15, 0, [zero], c8, c7, 0":: [zero] "r"(zero) );
        }

        void init(KLib::Logger&, MemoryRegion* regions, size_t nb);

    private:
        void _initAllocators(size_t addr, MemoryRegion* regions, size_t nb);
    };
}

#endif /* MEMORY_HPP_ */
