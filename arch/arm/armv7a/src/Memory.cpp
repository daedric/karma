#include "cpu/Memory.hpp"
#include "klib/Register.hpp"

using namespace CPU;

extern "C"
{
    extern char __kernel_end_mapping__;
}

static constexpr void* get_kernel_end()
{
    return reinterpret_cast<void*>(size_t(&__kernel_end_mapping__));
}


typedef unsigned int page_entry;
typedef page_entry L1Directory[4096];
typedef page_entry L2Directory4k[256];
typedef page_entry L2Directory64k[16];

static_assert(sizeof(L1Directory) == 16_Ko, "Invalid L1 page table size");

enum SectionRegisterPart
{
    SECTION_BASE_ADDR,
    SECTION_EXECUTE_NEVER,
    SECTION_SBZ,
    SECTION_TEX,
    SECTION_AP,
    SECTION_IMP,
    SECTION_DOMAIN,
    SECTION_AP2,
    SECTION_NG,
    SECTION_NS,
    SECTION_SHARED,
    SECTION_CACHE,
    SECTION_BUFFER,
    SECTION_TYPE,
    SECTION_IS_SUPERSECTION,
};

enum TypeValue
{
    SECTION_ENTRY_TYPE = 1 << 1,
    COARSE_ENTRY_TYPE = 1
};

enum SmallPage
{
    PAGE_BASE_ADDRESS,
    PAGE_EXECUTE_NEVER,
    PAGE_TEX,
    PAGE_SHARED,
    PAGE_AP,
    PAGE_AP2,
    PAGE_NG,
    PAGE_CACHE,
    PAGE_BUFFER,
    PAGE_TYPE,
    PAGE_SBZ,
};

enum SmallPageValue
{
    SMALL_PAGE_TYPE = 1 << 1
};

// definition present at the page 1317 (section B3) from ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition
typedef KLib::Register<unsigned int, KLib::Address<>,  KLib::RegisterPart<SECTION_TYPE, 1>,
                                                        KLib::RegisterPart<SECTION_BUFFER, 2>,
                                                        KLib::RegisterPart<SECTION_CACHE, 3>,
                                                        KLib::RegisterPart<SECTION_EXECUTE_NEVER, 4>,
                                                        KLib::RegisterPart<SECTION_DOMAIN, 5, 9>,
                                                        KLib::RegisterPart<SECTION_IMP, 9>,
                                                        KLib::RegisterPart<SECTION_AP, 10, 12>,
                                                        KLib::RegisterPart<SECTION_TEX, 12, 15>,
                                                        KLib::RegisterPart<SECTION_AP2, 15>,
                                                        KLib::RegisterPart<SECTION_SHARED, 16>,
                                                        KLib::RegisterPart<SECTION_NG, 17>,
                                                        KLib::RegisterPart<SECTION_IS_SUPERSECTION, 18>,
                                                        KLib::RegisterPart<SECTION_NS, 19>,
                                                        KLib::RegisterPart<SECTION_BASE_ADDR, 20, 32> > SectionEntry;

typedef KLib::Register<unsigned int, KLib::Address<>,  KLib::RegisterPart<SECTION_TYPE, 0, 2>,
                                                        KLib::RegisterPart<SECTION_SBZ, 2>,
                                                        KLib::RegisterPart<SECTION_NS, 3>,
                                                        KLib::RegisterPart<SECTION_SBZ, 4>,
                                                        KLib::RegisterPart<SECTION_DOMAIN, 5, 9>,
                                                        KLib::RegisterPart<SECTION_SBZ, 9>,
                                                        KLib::RegisterPart<SECTION_BASE_ADDR, 10, 32> > PageTableEntry;

typedef KLib::Register<unsigned int, KLib::Address<>,  KLib::RegisterPart<PAGE_EXECUTE_NEVER, 0>,
                                                        KLib::RegisterPart<PAGE_TYPE, 1>,
                                                        KLib::RegisterPart<PAGE_BUFFER, 2>,
                                                        KLib::RegisterPart<PAGE_CACHE, 3>,
                                                        KLib::RegisterPart<PAGE_AP, 4, 6>,
                                                        KLib::RegisterPart<PAGE_TEX, 6, 9>,
                                                        KLib::RegisterPart<PAGE_AP2, 9>,
                                                        KLib::RegisterPart<PAGE_SHARED, 10>,
                                                        KLib::RegisterPart<PAGE_NG, 11>,
                                                        KLib::RegisterPart<PAGE_BASE_ADDRESS, 12, 32> > SmallPageEntry;

typedef KLib::Register<unsigned int, KLib::Address<>, KLib::RegisterPart<PAGE_TYPE, 0, 2>,
                                                        KLib::RegisterPart<PAGE_BUFFER, 2>,
                                                        KLib::RegisterPart<PAGE_CACHE, 3>,
                                                        KLib::RegisterPart<PAGE_AP, 4, 6>,
                                                        KLib::RegisterPart<PAGE_SBZ, 6, 9>,
                                                        KLib::RegisterPart<PAGE_AP2, 9>,
                                                        KLib::RegisterPart<PAGE_SHARED, 10>,
                                                        KLib::RegisterPart<PAGE_NG, 11>,
                                                        KLib::RegisterPart<PAGE_TEX, 12, 15>,
                                                        KLib::RegisterPart<PAGE_EXECUTE_NEVER, 15>,
                                                        KLib::RegisterPart<PAGE_BASE_ADDRESS, 16, 32> > LargePageEntry;


static inline void set_translation_table_address(L1Directory* l1_page_table)
{
    asm volatile("MCR p15, 0, %[pt], c2, c0, 0"::[pt] "r"(l1_page_table) : "memory");
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


MMU::MMU()
{
}

MMU::~MMU(){}


static void init4kl2(page_entry& entry, size_t& addr, size_t& current_phys_addr)
{
    PageTableEntry pte(&entry);
    pte.setSlicedValue<SECTION_BASE_ADDR>(addr)
            .setValue<SECTION_TYPE>(1);
    L2Directory4k* l2_directory = reinterpret_cast<L2Directory4k*>(addr);

    for (page_entry& entry_l2 : *l2_directory)
    {
        SmallPageEntry page(&entry_l2);
        page.writeValue(0);
        page.setValue<PAGE_TYPE>(1)
                .setValue<PAGE_AP>(3)
                .setSlicedValue<PAGE_BASE_ADDRESS>(current_phys_addr);
        current_phys_addr += 4_Ko;
    }

    addr += 1_Ko;
}

static void init16kl2(page_entry& entry, size_t& addr, size_t& current_phys_addr)
{
    PageTableEntry pte(&entry);
    pte.setSlicedValue<SECTION_BASE_ADDR>(addr)
            .setValue<SECTION_TYPE>(1);
    L2Directory64k* l2_directory = reinterpret_cast<L2Directory64k*>(addr);

    for (page_entry& entry_l2 : *l2_directory)
    {
        LargePageEntry page(&entry_l2);
        page.writeValue(0);
        page.setValue<PAGE_TYPE>(1)
                .setValue<PAGE_AP>(3)
                .setSlicedValue<PAGE_BASE_ADDRESS>(current_phys_addr);
        current_phys_addr += 64_Ko;
    }

    addr += 1_Ko; // must be aligned on 1k
}

static void init1msection(page_entry& entry, size_t&, size_t& current_phys_addr)
{
    SectionEntry sec(&entry);
    sec.setSlicedValue<SECTION_BASE_ADDR>(current_phys_addr)
            .setValue<SECTION_TYPE>(1)
            .setValue<SECTION_AP>(3);
    current_phys_addr += 1_Mo;
}

void MMU::init(KLib::Logger& logger, MemoryRegion* regions, size_t nb)
{

    logger << "MMU: Initialize L1" << KLib::endl;

    size_t addr = size_t(get_kernel_end());

    // align the address if needed
    if ((addr & 0xFFFFC000) != addr)
    {
        addr += 16_Ko - (addr & ~0xFFFFC000);
    }

    logger << "Start to map memory page table at: " << addr << KLib::endl;
    L1Directory* l1_page_table_ptr = reinterpret_cast<L1Directory*>(addr);
    L1Directory& l1_page_table = *l1_page_table_ptr;

    addr += sizeof(L1Directory);

    for (auto& entry : l1_page_table) { entry = 0; } // page fault by default

    void (*init_page_func)(page_entry&, size_t&, size_t&) = nullptr;
    constexpr size_t increment = 1_Mo;

    size_t regions_nb = nb;
    for (MemoryRegion* current_region = regions; regions_nb; ++current_region, --regions_nb)
    {
        size_t physical_region_start = current_region->physical_base_address;
        size_t physical_region_end = physical_region_start + current_region->size;

        if (physical_region_end < physical_region_start)
        {
            logger << "Size is too big region_end < region_start, likely caused by an overflow" << KLib::endl;
            while (1);
        }

        size_t virt_region_start = current_region->virtual_base_address;
        size_t virt_region_end = current_region->virtual_base_address + current_region->size;

        if (virt_region_end < virt_region_start)
        {
            logger << "Size is too big virt_region_end < virt_region_start, likely caused by an overflow" << KLib::endl;
            while (1);
        }

        logger << "Region start: " << physical_region_start << ", region_end: "
                << physical_region_end << ", VStart at: " << virt_region_start
                << ", VEnd at: " << virt_region_end << KLib::endl;

        switch (current_region->page_size)
        {
            case PageSize::P4k:
                init_page_func = init4kl2;
                break;
            case PageSize::P64k:
                init_page_func = init16kl2;
                break;
            case PageSize::P1M:
                init_page_func = init1msection;
                break;
        }

        for (size_t current_phys_addr = physical_region_start; current_phys_addr < physical_region_end; virt_region_start += increment)
        {
            page_entry& entry = l1_page_table[virt_region_start >> 20];
            init_page_func(entry, addr, current_phys_addr);
        }

    }

    logger << "Activate MMU" << KLib::endl;

    set_translation_table_address(l1_page_table_ptr);
    enable_mmu();
    logger << "MMU initialized" << KLib::endl;
}
