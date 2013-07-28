#include "klib/logger/LoggerFormat.hpp"

using namespace KLib;

static const char* base_char = "0123456789abcdefghijklnmopqrstuvwxyz";
static const char* prefix_base[] =
{
    nullptr, nullptr, "b", nullptr, nullptr, nullptr, nullptr, nullptr, "0",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "0x",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr
};


LoggerFormat::LoggerFormat()
: default_number_base_(10)
{
}

void LoggerFormat::setDefaultNumberBase(unsigned int base)
{
    if (base > 1 && base < kstrlen(base_char))
    {
        default_number_base_ = base;
    }
}

unsigned int LoggerFormat::getDefaultNumberBase() const
{
    return default_number_base_;
}

const char* LoggerFormat::getBasePrefix(unsigned int base) const
{
    return prefix_base[base];
}
