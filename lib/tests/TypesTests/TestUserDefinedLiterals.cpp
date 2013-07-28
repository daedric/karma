#include <cassert>
#include <iostream>
#include <utility>
#include "klib/Types.h"

int main(int, char**)
{
    assert(1_Go == 1024 * 1024 * 1024);
    return 0;
}
