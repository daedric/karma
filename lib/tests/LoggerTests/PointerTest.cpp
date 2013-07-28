#include "klib/logger/Logger.hpp"

#include <iostream>
#include <iomanip>
#include <limits>
#include <cstring>
#include <cassert>
#include <sstream>

using namespace KLib;

struct MyDriver : public Driver
{
    MyDriver()
    : Driver(Driver::CAN_WRITE)
    {}

    void device_write(const char* str, size_t len)
    {
        std::cout.write(str, len);
        str_.append(str, len);
    }

    std::string str_;
};

template <typename T>
std::string to_str(T t)
{
    std::ostringstream str;
    str << std::hex << t;
    return str.str();
}

template <typename T>
void test_case(Logger& logger, MyDriver& driver, T value)
{
    std::string expected = to_str(value);
    driver.str_.clear();
    logger << dump_data(value);

    if (driver.str_ != expected)
    {
        std::cout << "\n" << driver.str_ << " != " << expected << std::endl;
        abort();
    }
    logger << endl;
}

int main(int, char**)
{
    MyDriver driver;
    Logger log(&driver);

# define test(value) test_case(log, driver, value)

    test(((int*)0xDEADBEEF));
    return 0;
}
