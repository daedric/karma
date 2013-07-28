#include "klib/logger/Logger.hpp"

#include <iostream>
#include <cstring>

using namespace KLib;

const char* data = "coucou ca va ?";

struct MyDriver : public Driver
{
    MyDriver()
    : Driver(Driver::CAN_WRITE)
    {}

    void device_write(const char* str, size_t len)
    {
        if (str != data)
        {
            std::cout << "Invalid string";
        }
        std::cout.write(str, len);
    }
};

struct test
{
    void dump(Logger& logger) const
    {
        logger.write(data);
    }
};

static_assert(KLib::detail::HasDoubleRedirectionOperator<KLib::Logger, const char*>::value, "Logger << const char* not resolved");
static_assert(KLib::detail::HasDoubleRedirectionOperator<KLib::Logger, char>::value, "Logger << char");

int main(int, char**)
{
    {
        Logger log(new MyDriver);
        log << dump_data(data);
        log << dump_data(test());
        log << data;
        log << test();
    }

    return 0;
}
