#ifndef LOGGERFORMAT_HPP_
# define LOGGERFORMAT_HPP_

# include "klib/String.hpp"

namespace KLib
{

class LoggerFormat
{
public:
    LoggerFormat();
    void setDefaultNumberBase(unsigned int base);
    unsigned int getDefaultNumberBase() const;
    const char* getBasePrefix(unsigned int base) const;

private:
    unsigned int    default_number_base_;
};

}


#endif /* LOGGERFORMAT_HPP_ */
