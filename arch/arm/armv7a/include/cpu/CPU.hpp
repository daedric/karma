#ifndef __CPU_HPP__
# define __CPU_HPP__

# include "common/CPU.hpp"
# include "klib/NonCopyable.hpp"
# include "klib/Logger.hpp"
# include "Memory.hpp"

namespace CPU
{

    namespace detail
    {

        enum
        {
            GENERAL = -1,
        };

        enum SCR
        {
            INTR_VECTOR = 0
        };

        enum CPSR
        {
            MODE = 0, THUMB, FIQ, IRQ, FIQIRQ,
        };

        typedef KLib::Register<unsigned int, KLib::Address<>,
                        KLib::RegisterPart<GENERAL, 0, 32>,
                        KLib::RegisterPart<INTR_VECTOR, 13> > SystemControlRegister;

        typedef KLib::Register<unsigned int, KLib::Address<>,
                                                KLib::RegisterPart<GENERAL, 0, 32>,
                                                KLib::RegisterPart<MODE, 0, 5>,
                                                KLib::RegisterPart<THUMB, 5>,
                                                KLib::RegisterPart<FIQ, 6>,
                                                KLib::RegisterPart<IRQ, 7>,
                                                KLib::RegisterPart<FIQIRQ, 6, 8> > CPSRegister;

        static inline unsigned int getSystemControlRegister()
        {
            unsigned int reg;
            asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
            return reg;
        }

        static inline unsigned int getCurrentProgramStatusRegister()
        {
            unsigned int reg;
            asm volatile("mrs %0, cpsr" : "=r" (reg) ::);
            return reg;
        }

        static inline void writeSystemControlRegister(SystemControlRegister& reg)
        {
            unsigned int value = reg.getValue<GENERAL, unsigned int>();
            asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (value) : "cc");
        }

        enum class CPSR_REGISTER_PART
        {
            PART_C, PART_X, PART_S, PART_F, ALL
        };

        template<CPSR_REGISTER_PART part = CPSR_REGISTER_PART::PART_C>
        static inline void writeCurrentProgramStatusRegister(CPSRegister& reg)
        {
            unsigned int value = reg.getValue<GENERAL, unsigned int>();

            switch (part)
            {
                case CPSR_REGISTER_PART::PART_C:
                    asm volatile("msr cpsr_c, %0" : : "r"(value) : "memory");
                    return;
                case CPSR_REGISTER_PART::PART_X:
                    asm volatile("msr cpsr_x, %0" : : "r"(value) : "memory");
                    return;
                case CPSR_REGISTER_PART::PART_S:
                    asm volatile("msr cpsr_s, %0" : : "r"(value) : "memory");
                    return;
                case CPSR_REGISTER_PART::PART_F:
                    asm volatile("msr cpsr_f, %0" : : "r"(value) : "memory");
                    return;
                case CPSR_REGISTER_PART::ALL:
                    asm volatile("msr cpsr, %0"   : : "r"(value) : "memory");
                    return;
            }
        }

# define GET_SYSTEM_CONTROL_REGISTER(var)       \
        unsigned int register_ ## var = detail::getSystemControlRegister();         \
        detail::SystemControlRegister var(&register_ ## var)

# define GET_CPSREGISTER(var)       \
        unsigned int cpsr_register_ ## var = detail::getCurrentProgramStatusRegister();         \
        detail::CPSRegister var(&cpsr_register_ ## var)
    }

    class ARMV7a: public ARM::CPU
                , private KLib::NonCopyable
    {
    public:
        ARMV7a(KLib::Logger& logger);
        ~ARMV7a();

        void setupInterruptHandlers();

        void setHiVec()
        {
            GET_SYSTEM_CONTROL_REGISTER (ctrl);
            ctrl.setValue<detail::INTR_VECTOR>(1);
            detail::writeSystemControlRegister(ctrl);
        }

        void setLowVec()
        {
            GET_SYSTEM_CONTROL_REGISTER (ctrl);
            ctrl.setValue<detail::INTR_VECTOR>(0);
            detail::writeSystemControlRegister(ctrl);
        }

        size_t getInterruptVectorAddress() const
        {
            size_t addr = 0;
            {
                GET_SYSTEM_CONTROL_REGISTER (ctrl);
                if (ctrl.getValue<detail::INTR_VECTOR, unsigned int>()) // V bit == 1
                {
                    addr = (0xFFFF0000);
                }
            }
            return addr;
        }

        bool disableInterrupts()
        {
            GET_CPSREGISTER(cspr);
            unsigned int value = cspr.getValue<detail::FIQIRQ, unsigned int>();
            cspr.setValue<detail::FIQ>(1)
                    .setValue<detail::IRQ>(1);
            detail::writeCurrentProgramStatusRegister<>(cspr);
            return value == 0;
        }

        void enableInterrupts()
        {
            GET_CPSREGISTER(cspr);
            cspr.setValue<detail::FIQ>(0)
                    .setValue<detail::IRQ>(0);
            detail::writeCurrentProgramStatusRegister<>(cspr);
        }


    private:
        inline KLib::Logger& get_logger()
        {
            return this->logger_;
        }

    };

    typedef ARMV7a CurrentCPU;
}

#endif /* __CPU_HPP__ */
