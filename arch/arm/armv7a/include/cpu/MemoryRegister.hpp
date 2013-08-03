#ifndef MEMORYREGISTER_HPP_
# define MEMORYREGISTER_HPP_

# include "klib/compiler/compiler.hpp"
# include "klib/Register.hpp"

namespace CPU
{

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

typedef KLib::Register<unsigned int, KLib::Address<>,
        KLib::RegisterPart<SECTION_TYPE, 0, 2>,
        KLib::RegisterPart<SECTION_SBZ, 2>,
        KLib::RegisterPart<SECTION_NS, 3>,
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
        KLib::RegisterPart<PAGE_TYPE, 1>,
        KLib::RegisterPart<PAGE_BUFFER, 2>,
        KLib::RegisterPart<PAGE_CACHE, 3>,
        KLib::RegisterPart<PAGE_AP, 4, 6>,
        KLib::RegisterPart<PAGE_TEX, 6, 9>,
        KLib::RegisterPart<PAGE_AP2, 9>,
        KLib::RegisterPart<PAGE_SHARED, 10>,
        KLib::RegisterPart<PAGE_NG, 11>,
        KLib::RegisterPart<PAGE_BASE_ADDRESS, 12, 32> > SmallPageEntry;

typedef KLib::Register<unsigned int, KLib::Address<>,
        KLib::RegisterPart<PAGE_TYPE, 0, 2>, KLib::RegisterPart<PAGE_BUFFER, 2>,
        KLib::RegisterPart<PAGE_CACHE, 3>, KLib::RegisterPart<PAGE_AP, 4, 6>,
        KLib::RegisterPart<PAGE_SBZ, 6, 9>, KLib::RegisterPart<PAGE_AP2, 9>,
        KLib::RegisterPart<PAGE_SHARED, 10>, KLib::RegisterPart<PAGE_NG, 11>,
        KLib::RegisterPart<PAGE_TEX, 12, 15>,
        KLib::RegisterPart<PAGE_EXECUTE_NEVER, 15>,
        KLib::RegisterPart<PAGE_BASE_ADDRESS, 16, 32> > MediumPageEntry;

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
} __align__(1_Ko);

struct MediumPage
{
    typedef PageEntry pages[16];

    PageEntry& operator[](size_t index)
    {
        return pages_[index];
    }

    pages pages_;
} __align__(1_Ko);

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

} __align__(16_Ko);

}

#endif /* MEMORYREGISTER_HPP_ */
