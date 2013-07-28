#include "klib/logger/LoggerOverload.hpp"

namespace KLib
{
Logger& endl(Logger& logger)
{
    logger.endl();
    return logger;
}

Logger& operator<<(Logger& logger, Logger& (*func)(Logger&))
{
    return func(logger);
}

Logger& operator<<(Logger& logger, const char* str)
{
    return logger.write(str);
}

# define GENERATE_DUMPER(type)                      \
        Logger& operator<<(Logger& logger, type i)  \
        {                                           \
            return logger << Dumper<type>(i);       \
        }

GENERATE_DUMPER(int);
GENERATE_DUMPER(unsigned int);
GENERATE_DUMPER(char);
GENERATE_DUMPER(unsigned char);
GENERATE_DUMPER(short);
GENERATE_DUMPER(unsigned short);
#undef GENERATE_DUMPER

}
