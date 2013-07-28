#ifndef _ARM_COMMON_CPU_HPP_
# define _ARM_COMMON_CPU_HPP_

# include "klib/Logger.hpp"

namespace ARM
{
class CPU
{
public:

    bool disableInterrupts();
    void enableInterrupts();

protected:
    CPU(KLib::Logger& logger);
    ~CPU();
    KLib::Logger& logger_;
};

}
#endif /* _ARM_COMMON_CPU_HPP_ */
