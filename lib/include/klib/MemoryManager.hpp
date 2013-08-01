#ifndef MEMORYMANAGER_HPP_
# define MEMORYMANAGER_HPP_

# include "Traits.hpp"
# include "Utils.hpp"
# include "Types.h"

namespace KLib
{

enum class PageSize
{
    SmallPage = 4_Ko,
    MediumPage = 64_Ko,
    LargePage = 1_Mo,
};

enum class MemoryType
{
    Device,
    Kernel,
    RAM
};


namespace detail
{
template <typename Directory, PageSize value>
    struct GetPageFromSize;


template <typename Directory>
struct GetPageFromSize<Directory, PageSize::LargePage>
{
    using type = typename Directory::L1LargePage;
};

template <typename Directory>
struct GetPageFromSize<Directory, PageSize::MediumPage>
{
    using type = typename Directory::L2MediumPage;
};

template <typename Directory>
struct GetPageFromSize<Directory, PageSize::SmallPage>
{
    using type = typename Directory::L2SmallPage;
};
}


template <
    size_t PA_BASE,
    size_t SIZE,
    size_t VA_BASE,
    PageSize PSIZE,
    MemoryType MTYPE
>
struct MemoryRegion
{
    static constexpr size_t physical_base_address = PA_BASE;
    static constexpr size_t size                  = SIZE;
    static constexpr size_t virtual_base_address  = VA_BASE;
    static constexpr PageSize page_size           = PSIZE;
    static constexpr MemoryType memory_type       = MTYPE;

    template <typename Directory>
    static constexpr auto getDirectory() -> decltype(Directory::template getPageFromSize<page_size>());
};

template <
    typename MemoryHandler
>
struct Directory
{
    using L1Directory = typename MemoryHandler::L1Directory;
    using L1LargePage = typename MemoryHandler::L1LargePage;
    using L2SmallPage = typename MemoryHandler::L2SmallPage;
    using L2MediumPage = typename MemoryHandler::L2MediumPage;

    template <PageSize size>
    static constexpr auto getPageFromSize() -> typename detail::GetPageFromSize<Directory, size>::type;
};

namespace detail
{
// from an idea: http://stackoverflow.com/a/16853775
template<typename, typename>
struct append { };

template<typename T, typename... ListTypes, template<typename...> class Holder>
struct append<T, Holder<ListTypes...>>
{
    using type = Holder<ListTypes..., T>;
};

template <MemoryType type, typename Holder, typename... Regions>
struct FilterRegions;

template <MemoryType type, typename Holder, typename Region, typename... Regions>
struct FilterRegions<type, Holder, Region, Regions...>
{
    using list = typename Traits::static_if_c<Region::memory_type == type,
                                                typename append<Region, typename FilterRegions<type, Holder, Regions...>::list>::type,
                                                typename FilterRegions<type, Holder, Regions...>::list>::type;
};

template <MemoryType type, typename Holder>
struct FilterRegions<type, Holder>
{
    using list = Holder;
};
}

namespace detail
{

template <typename... Regions>
struct RamPages
{
};

template <typename... Regions>
struct KernelPages
{

};

template <typename... Regions>
struct DevicePages
{

};


}

template <
    typename MemoryHandler,

    typename ...Regions
>
class MemoryManager
{
    using DirectoryType = Directory<MemoryHandler>;

public:
    using RamPages    = typename detail::FilterRegions<MemoryType::RAM,    detail::RamPages<>,    Regions...>::list;
    using KernelPages = typename detail::FilterRegions<MemoryType::Kernel, detail::KernelPages<>, Regions...>::list;
    using DevicePages = typename detail::FilterRegions<MemoryType::Device, detail::DevicePages<>, Regions...>::list;




    typename DirectoryType::L1Directory main_directory_;
};



}


#endif /* MEMORYMANAGER_HPP_ */
