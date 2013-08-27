#ifndef MEMORYMANAGER_HPP_
# define MEMORYMANAGER_HPP_

# include "Traits.hpp"
# include "Utils.hpp"
# include "Types.h"

namespace KLib
{

enum class PageSize
{
    SmallPage = 4_Ko, MediumPage = 64_Ko, LargePage = 1_Mo,
};

enum class MemoryType
{
    Device, Kernel, RAM
};

namespace detail
{
template<typename Directory, PageSize value>
struct GetPageFromSize;

template<typename Directory>
struct GetPageFromSize<Directory, PageSize::LargePage>
{
    using type = typename Directory::LargePage;
};

template<typename Directory>
struct GetPageFromSize<Directory, PageSize::MediumPage>
{
    using type = typename Directory::MediumPage;
};

template<typename Directory>
struct GetPageFromSize<Directory, PageSize::SmallPage>
{
    using type = typename Directory::SmallPage;
};
}

template<
        size_t PA_BASE,
        size_t SIZE,
        size_t VA_BASE,
        PageSize PSIZE,
        MemoryType MTYPE,
        size_t CHUNK=1_Mo
>
struct MemoryRegion
{
    static_assert(SIZE % CHUNK == 0, "Size must be a multiple of a 1 Mo");

    static constexpr size_t memory_chunk = CHUNK;
    static constexpr size_t physical_base_address = PA_BASE;
    static constexpr size_t size = SIZE;
    static constexpr size_t virtual_base_address = VA_BASE;
    static constexpr PageSize page_size = PSIZE;
    static constexpr size_t nb_pages = SIZE / size_t(PSIZE);
    static constexpr size_t nb_directories = SIZE / 1_Mo;
    static constexpr MemoryType memory_type = MTYPE;
};

template<typename MemoryHandler>
struct Directory : private KLib::NonCopyable
{
    using LargePage = typename MemoryHandler::LargePage;
    using SmallPage = typename MemoryHandler::SmallPage;
    using MediumPage = typename MemoryHandler::MediumPage;

    template<PageSize size>
    static constexpr auto getPageFromSize() -> typename detail::GetPageFromSize<Directory, size>::type;

    template <typename PageType>
    static auto AllocatePage(PageType& page, size_t base_pa, size_t pa, size_t va)
    -> decltype(MemoryHandler::AllocatePage(page, base_pa, pa, va))
    { return MemoryHandler::AllocatePage(page, base_pa, pa, va); }
};

namespace detail
{
// from an idea: http://stackoverflow.com/a/16853775
template<typename, typename >
struct append
{
};

template<typename T, typename ... ListTypes, template<typename ...> class Holder>
struct append<T, Holder<ListTypes...>>
{
    using type = Holder<ListTypes..., T>;
};

template<MemoryType type, typename Holder, typename ... Regions>
struct FilterRegions;

template<MemoryType type, typename Holder, typename Region, typename ... Regions>
struct FilterRegions<type, Holder, Region, Regions...>
{
    using list = typename Traits::static_if_c<Region::memory_type == type,
    typename append<Region, typename FilterRegions<type, Holder, Regions...>::list>::type,
    typename FilterRegions<type, Holder, Regions...>::list>::type;
};

template<MemoryType type, typename Holder>
struct FilterRegions<type, Holder>
{
    using list = Holder;
};
}

namespace detail
{

template<typename Directory, typename ... Region>
struct PagesHandler;

template<typename Directory, typename Region, typename ... Regions>
struct PagesHandler<Directory, Region, Regions...> : public PagesHandler<Directory, Regions...>, private KLib::NonCopyable
{
    using RegionPageType = decltype(Directory::template getPageFromSize<Region::page_size>());
    using Base = PagesHandler<Directory, Regions...>;

    struct PageStates : private KLib::NonCopyable
    {
        bool is_free;
        size_t base_pa;
        size_t pa;
        size_t va;
        size_t size;

        RegionPageType* page;
        PageStates* previous;
    };

    PagesHandler()
    {
        size_t pa = Region::physical_base_address,
                va = Region::virtual_base_address,
                cummulated_size = 0;
        const size_t size = size_t(Region::page_size);

        const size_t base_pa = Region::physical_base_address;
        const size_t chunk = Region::memory_chunk;

        for (size_t i = 0; i < Region::nb_pages; ++i)
        {
            auto& state = pages_state[i];
            auto const current_page = cummulated_size / chunk;

            state.is_free = true;
            state.base_pa = base_pa + chunk * current_page;
            state.pa = pa;
            state.va = va;
            state.size = size;
            state.page = &(pages_[current_page]);

            pa += size;
            va += size;
            cummulated_size += size;
        }

        PageStates* previous = nullptr;
        for (int i = Region::nb_pages - 1; i > -1; --i)
        {
            auto& state = pages_state[i];
            state.previous = previous;
            previous = &state;
        }

        free_pages = previous;
        used_pages = nullptr;

        memset(pages_, 0, sizeof(pages_));
    }

    bool allocate(void*& memory_start, size_t& page_size)
    {
        if (free_pages)
        {
            auto& state = *free_pages;
            state.is_free = false;
            bool success = Directory::AllocatePage(*state.page,
                                                   state.base_pa,
                                                   state.pa,
                                                   state.va);
            if (success)
            {
                memory_start = reinterpret_cast<void*>(state.va);
                page_size = state.size;
            }

            auto prev = free_pages->previous;
            free_pages->previous = used_pages;
            used_pages = free_pages;
            free_pages = prev;
            return success;
        }

        return Base::allocate(memory_start, page_size);
    }

    RegionPageType pages_[Region::nb_directories];
    PageStates* free_pages;
    PageStates* used_pages;
    PageStates pages_state[Region::nb_pages];
};


template<typename Directory>
struct PagesHandler<Directory>
{
    bool allocate(void*& memory_start, size_t& page_size)
    {
        memory_start = nullptr;
        page_size = 0;
        return false;
    }
};

template<typename Directory, typename ... Regions>
struct RamPages: public PagesHandler<Directory, Regions...>
{

};

template<typename Directory, typename ... Regions>
struct KernelPages: public PagesHandler<Directory, Regions...>
{

};

template<typename Directory, typename ... Regions>
struct DevicePages: public PagesHandler<Directory, Regions...>
{

};

}

template<typename MemoryHandler, typename ...Regions>
class MemoryManager : private KLib::NonCopyable
{
public:

    using DirectoryType = Directory<MemoryHandler>;
    using RamPages = typename detail::FilterRegions<MemoryType::RAM, detail::RamPages<DirectoryType>, Regions...>::list;
    using KernelPages = typename detail::FilterRegions<MemoryType::Kernel, detail::KernelPages<DirectoryType>, Regions...>::list;
    using DevicePages = typename detail::FilterRegions<MemoryType::Device, detail::DevicePages<DirectoryType>, Regions...>::list;



    bool allocateRAMPage(void*& memory, size_t& size)
    {
        return ram_.allocate(memory, size);
    }

    bool allocateKernelPage(void*& memory, size_t& size)
    {
        return kernel_.allocate(memory, size);
    }

    bool allocateDevicePage(void*& memory, size_t& size)
    {
        return devices_.allocate(memory, size);
    }

    RamPages ram_;
    KernelPages kernel_;
    DevicePages devices_;
};

}

#endif /* MEMORYMANAGER_HPP_ */
