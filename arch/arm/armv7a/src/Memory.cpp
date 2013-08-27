#include "cpu/Memory.hpp"
#include "klib/compiler/compiler.hpp"
#include "klib/MemoryManager.hpp"

using namespace CPU;

Directory __section__("data") MMU::l1;
KLib::Logger* MMU::logger = nullptr;

extern "C"
{
    extern char __kernel_end_mapping__;
}


void* MMU::get_kernel_end()
{
    return reinterpret_cast<void*>(size_t(&__kernel_end_mapping__));
}

static inline void set_translation_table_address(Directory* l1)
{
    asm volatile("MCR p15, 0, %[pt], c2, c0, 0"::[pt] "r"(l1) : "memory");
}

static inline void enable_mmu()
{
    unsigned int reg;
    asm volatile("mrc2 p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
    reg |= 0x1;
    asm volatile("mcr2 p15, 0, %0, c1, c0, 0" : : "r" (reg) : "cc");
}

static inline void disable_mmu()
{
    unsigned int reg;
    asm volatile("mrc2 p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
    reg &= ~size_t(0x1);
    asm volatile("mcr2 p15, 0, %0, c1, c0, 0" : : "r" (reg) : "cc");
}


void MMU::init()
{
    set_translation_table_address(&MMU::l1);

    logger_ << "Enable mmu" << KLib::endl;
    enable_mmu();
    logger_ << "MMU enabled" << KLib::endl;
}

void MMU::disable()
{
    disable_mmu();
}

bool MMU::AllocatePage(LargePage&,
                       size_t,
                       size_t pa,
                       size_t va)
{
    auto& l1_entry = l1[va];
    LargePageEntry lpe(&l1_entry);
    lpe.setSlicedValue<SECTION_BASE_ADDR>(pa)
            .setValue<SECTION_TYPE>(1)
            .setValue<SECTION_AP>(3);

    return true;
}

bool MMU::AllocatePage(SmallPage& page,
                       size_t base_pa,
                       size_t pa,
                       size_t va)
{
    auto& l1_entry = l1[va];

    if (!l1_entry)
    {
        *logger << "Create new page table entry" << KLib::endl;
        PageTableEntry pte(&l1_entry);
        pte.setSlicedValue<SECTION_BASE_ADDR>(size_t(&page))
                .setValue<SECTION_TYPE>(1);
    }

    auto index = (pa - base_pa) / size_t(KLib::PageSize::SmallPage);
    auto& entry = page[index];

    SmallPageEntry spe(&entry);
    spe.setValue<PAGE_TYPE>(1)
            .setValue<PAGE_AP>(3)
            .setSlicedValue<PAGE_BASE_ADDRESS>(pa);

    return true;
}

bool MMU::AllocatePage(MediumPage& page,
                       size_t base_pa,
                       size_t pa,
                       size_t va)
{
    auto& l1_entry = l1[va];

    if (!l1_entry)
    {
        *logger << "Create new page table entry" << KLib::endl;
        PageTableEntry pte(&l1_entry);
        pte.setSlicedValue<SECTION_BASE_ADDR>(size_t(&page))
            .setValue<SECTION_TYPE>(1);
    }

    auto index = (pa - base_pa) / size_t(KLib::PageSize::MediumPage);
    auto& entry = page[index];

    MediumPageEntry mpe(&entry);
    mpe.setValue<PAGE_TYPE>(1)
            .setValue<PAGE_AP>(3)
            .setSlicedValue<PAGE_BASE_ADDRESS>(pa);

    return true;
}
