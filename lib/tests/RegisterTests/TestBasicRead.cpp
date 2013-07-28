#include <cassert>
#include "klib/Register.hpp"

using namespace KLib;

int main(int, char**)
{

    {
        unsigned int i = 0x11223344;

        Register<unsigned int, Address<>, RegisterPart<0, 0, 8>,
                RegisterPart<1, 8, 16>, RegisterPart<2, 16, 24>,
                RegisterPart<3, 24, 32>, RegisterPart<4, 4, 28> > reg(&i);

        assert((reg.getValue<0, unsigned int>()) == (i & 0x000000FF) >> 0);
        assert((reg.getValue<1, unsigned int>()) == (i & 0x0000FF00) >> 8);
        assert((reg.getValue<2, unsigned int>()) == (i & 0x00FF0000) >> 16);
        assert((reg.getValue<3, unsigned int>()) == (i & 0xFF000000) >> 24);
        assert((reg.getValue<4, unsigned int>()) == (i & 0x0FFFFFF0) >> 4);
    }

    {
        unsigned int i = 0x11223300;
        char* ptr = (char*) &i;

        Register<uint8_t, Address<1>,
        RegisterPart<0, 0>,
        RegisterPart<1, 1>,
        RegisterPart<2, 2>,
        RegisterPart<3, 3>,
        RegisterPart<4, 4>,
        RegisterPart<5, 5>,
        RegisterPart<6, 6>,
        RegisterPart<7, 7>
        > reg(ptr);

        i |= 1 << 5;

        assert((reg.getValue<5, unsigned char>() == 1));

        reg.setValue<5>(0);
    }

    return 0;
}
