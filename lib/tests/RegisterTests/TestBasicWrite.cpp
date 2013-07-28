#include <cassert>
#include <iostream>
#include <utility>
#include "klib/Register.hpp"

using namespace KLib;

int main(int, char**)
{

    {
        unsigned int j = 0x00000000;
        Register<unsigned int, Address<>
        , RegisterPart<0, 12, 32>
        > reg(&j);

        reg.setSlicedValue<0>(0x12345678);
        assert((reg.getValue<0, unsigned int>() == 0x12345));

    }

    unsigned int i = 0x11223344;

    Register<unsigned int, Address<>
    , RegisterPart<0, 0, 8>
    , RegisterPart<1, 8, 16>
    , RegisterPart<2, 16, 24>
    , RegisterPart<3, 24, 32>
    , RegisterPart<4, 4, 28>
    > reg(&i);

    reg.setValue<1>(0x66)
        .setValue<2>(0x77);

    std::cout << std::hex << i << std::endl;

    assert((reg.getValue<1, unsigned int>()) == 0x66);
    assert((reg.getValue<2, unsigned int>()) == 0x77);


    reg.setValue<4>(0);
    assert((reg.getValue<4, unsigned int>()) == 0);
    assert(i == 0x10000004);
    reg.writeValue(0);
    assert(i == 0);

    return 0;
}
