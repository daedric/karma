#include <cassert>
#include <iostream>
#include <utility>
#include <type_traits>
#include "klib/MemoryManager.hpp"

using namespace KLib;

struct MemoryHandler
{
    struct L1Directory{};
    struct L1LargePage{};
    struct L2SmallPage{};
    struct L2MediumPage{};
};

using RamRegion = MemoryRegion <
    0,
    10,
    0,
    PageSize::SmallPage,
    MemoryType::RAM
>;

using KernelRegion = MemoryRegion <
    0,
    10,
    0,
    PageSize::SmallPage,
    MemoryType::Kernel
>;

using DeviceRegion = MemoryRegion <
    0,
    10,
    0,
    PageSize::SmallPage,
    MemoryType::Device
>;

int main(int, char**)
{
    using M = MemoryManager<MemoryHandler, KernelRegion, RamRegion, DeviceRegion>;

    M m;
    (void)m;

    static_assert(std::is_same<M::RamPages, detail::RamPages<RamRegion>>::value, "Ram page error!");
    static_assert(std::is_same<M::KernelPages, detail::KernelPages<KernelRegion>>::value, "Kernel page !");
    static_assert(std::is_same<M::DevicePages, detail::DevicePages<DeviceRegion>>::value, "Device page!");

    return 0;
}
