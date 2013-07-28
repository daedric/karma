#ifndef LOGGER_LOGGER_HPP_
# define LOGGER_LOGGER_HPP_

# include "klib/Utils.hpp"
# include "klib/Traits.hpp"
# include "LoggerUtils.hpp"
# include "Driver.hpp"
# include "LoggerFormat.hpp"

namespace KLib
{

class Logger : public LoggerFormat
{
public:
    Logger(Driver* driver);

    Logger& write(char c)
    {
        return this->write(&c, 1);
    }

    Logger& write(const char* str)
    {
        return this->write(str, kstrlen(str));
    }

    Logger& write(const char* str, size_t len)
    {
        driver_->write(str, len);
        return *this;
    }

    void endl()
    {
        driver_->writeEscapeSequence();
    }

private:
    Driver* driver_;
};

}
# include "Dumper.hpp"
# include "LoggerOverload.hpp"

#endif /* LOGGER_LOGGER_HPP_ */
