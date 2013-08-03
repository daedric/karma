#include <cassert>
#include <iostream>
#include <utility>
#include <type_traits>
#include "klib/Register.hpp"
#include "klib/compiler/compiler.hpp"
#include "klib/MemoryManager.hpp"

using namespace KLib;

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
    SECTION_ENTRY_TYPE = 1 << 1, COARSE_ENTRY_TYPE = 1
};

enum PagePart
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

typedef KLib::Register<unsigned int, KLib::Address<>,
        KLib::RegisterPart<SECTION_TYPE, 0, 2>,
        KLib::RegisterPart<SECTION_SBZ, 2>, KLib::RegisterPart<SECTION_NS, 3>,
        KLib::RegisterPart<SECTION_SBZ, 4>,
        KLib::RegisterPart<SECTION_DOMAIN, 5, 9>,
        KLib::RegisterPart<SECTION_SBZ, 9>,
        KLib::RegisterPart<SECTION_BASE_ADDR, 10, 32> > PageTableEntry;

typedef KLib::Register<unsigned int, KLib::Address<>,
        KLib::RegisterPart<SECTION_TYPE, 1>,
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
        KLib::RegisterPart<SECTION_BASE_ADDR, 20, 32> > LargePageEntry;

typedef KLib::Register<unsigned int, KLib::Address<>,
        KLib::RegisterPart<PAGE_EXECUTE_NEVER, 0>,
        KLib::RegisterPart<PAGE_TYPE, 1>, KLib::RegisterPart<PAGE_BUFFER, 2>,
        KLib::RegisterPart<PAGE_CACHE, 3>, KLib::RegisterPart<PAGE_AP, 4, 6>,
        KLib::RegisterPart<PAGE_TEX, 6, 9>, KLib::RegisterPart<PAGE_AP2, 9>,
        KLib::RegisterPart<PAGE_SHARED, 10>, KLib::RegisterPart<PAGE_NG, 11>,
        KLib::RegisterPart<PAGE_BASE_ADDRESS, 12, 32> > SmallPageEntry;

typedef KLib::Register<unsigned int, KLib::Address<>,
        KLib::RegisterPart<PAGE_TYPE, 0, 2>, KLib::RegisterPart<PAGE_BUFFER, 2>,
        KLib::RegisterPart<PAGE_CACHE, 3>, KLib::RegisterPart<PAGE_AP, 4, 6>,
        KLib::RegisterPart<PAGE_SBZ, 6, 9>, KLib::RegisterPart<PAGE_AP2, 9>,
        KLib::RegisterPart<PAGE_SHARED, 10>, KLib::RegisterPart<PAGE_NG, 11>,
        KLib::RegisterPart<PAGE_TEX, 12, 15>,
        KLib::RegisterPart<PAGE_EXECUTE_NEVER, 15>,
        KLib::RegisterPart<PAGE_BASE_ADDRESS, 16, 32> > MediumPageEntry;

struct MemoryHandler
{
    typedef unsigned int PageEntry;

    struct LargePage
    {
        // not needed
    };

    struct SmallPage
    {
        typedef PageEntry pages[256];

        PageEntry& operator[](size_t index)
        {
            return pages_[index];
        }

        pages pages_;
    }__align__(1_Ko);

    struct MediumPage
    {
        typedef PageEntry pages[16];

        PageEntry& operator[](size_t index)
        {
            return pages_[index];
        }

        pages pages_;
    }__align__(1_Ko);

    struct Directory
    {
        typedef PageEntry pages[4096];

        PageEntry& getPageEntry(size_t va)
        {
            return pages_[va >> 20];
        }

        PageEntry& operator[](size_t va)
        {
            return getPageEntry(va);
        }

        pages pages_;

    }__align__(16_Ko);

    static bool AllocatePage(LargePage&,
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

    static bool AllocatePage(SmallPage& page,
                             size_t base_pa,
                             size_t pa,
                             size_t va)
    {
        auto& l1_entry = l1[va];

        if (!l1_entry)
        {
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

    static bool AllocatePage(MediumPage& page, size_t base_pa, size_t pa,
                             size_t va)
    {
        auto& l1_entry = l1[va];

        if (!l1_entry)
        {
            std::cout << "Enable new entry" << std::endl;
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

    static Directory l1;
};

MemoryHandler::Directory MemoryHandler::l1 =
{ 0 };

using RamRegion = MemoryRegion <0, 1_Go, 0, PageSize::SmallPage, MemoryType::RAM>;
using KernelRegion = MemoryRegion <0, 1_Go, 0, PageSize::LargePage, MemoryType::Kernel>;
using DeviceRegion = MemoryRegion <0, 1_Go, 0, PageSize::MediumPage, MemoryType::Device>;

using M = MemoryManager<
                            MemoryHandler,
                            RamRegion,
                            KernelRegion,
                            RamRegion,
                            DeviceRegion>;

using DirectoryType = M::DirectoryType;

M __section__("data") m;

static_assert(std::is_same<M::RamPages, detail::RamPages<DirectoryType, RamRegion, RamRegion>>::value, "Ram page error!");
static_assert(std::is_same<M::KernelPages, detail::KernelPages<DirectoryType, KernelRegion>>::value, "Kernel page !");
static_assert(std::is_same<M::DevicePages, detail::DevicePages<DirectoryType, DeviceRegion>>::value, "Device page!");

int main(int, char**)
{

    void* memory = nullptr;
    size_t size = 0;

    {
        for (size_t i = 0; i < RamRegion::nb_pages * 2; ++i)
        {
            if (!m.allocateRAMPage(memory, size))
            {
                std::cout << "Cannot allocate page" << std::endl;
                return -1;
            }
        }

        if (m.allocateRAMPage(memory, size))
        {
            std::cout << "Should not have been able to allocate page" << std::endl;
            return -1;
        }
    }


    {
        for (size_t i = 0; i < KernelRegion::nb_pages; ++i)
        {
            if (!m.allocateKernelPage(memory, size))
            {
                std::cout << "Cannot allocate Kernel page" << std::endl;
                return -1;
            }
        }

        if (m.allocateKernelPage(memory, size))
        {
            std::cout << "Should not have been able to allocate kernel page" << std::endl;
            return -1;
        }
    }


    {
        for (size_t i = 0; i < DeviceRegion::nb_pages; ++i)
        {
            if (!m.allocateDevicePage(memory, size))
            {
                std::cout << "Cannot allocate Kernel page" << std::endl;
                return -1;
            }
        }

        if (m.allocateDevicePage(memory, size))
        {
            std::cout << "Should not have been able to allocate kernel page" << std::endl;
            return -1;
        }
    }

    return 0;
}
