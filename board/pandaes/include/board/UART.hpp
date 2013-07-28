#ifndef UART_HPP_
# define UART_HPP_

# include "klib/Types.h"
# include "klib/Address.hpp"
# include "klib/Register.hpp"
# include "klib/Logger.hpp"

# include "common/Memory.hpp" // operator needed

namespace Board
{
    typedef KLib::Address<1, true, true, true> UARTBaseAddress;

    namespace detail
    {
        enum DataRegisterPart { THR = 0, RHR = 0, DLL = 0 };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress,
                                KLib::RegisterPart<THR, 0, 8>> DataRegister;

        enum IERRegisterPart
        {
            RHR_IT = 0,
            THR_IT,
            LINE_STS_IT,
            MODEM_STS_IT,
            SLEEP_MODE,
            XOFF_IT,
            RTS_IT,
            CTS_IT,
            DLH,
        };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress,
                                KLib::RegisterPart<RHR_IT, 0>,
                                KLib::RegisterPart<THR_IT, 1>,
                                KLib::RegisterPart<LINE_STS_IT, 2>,
                                KLib::RegisterPart<MODEM_STS_IT, 3>,
                                KLib::RegisterPart<SLEEP_MODE, 4>,
                                KLib::RegisterPart<XOFF_IT, 5>,
                                KLib::RegisterPart<RTS_IT, 6>,
                                KLib::RegisterPart<CTS_IT, 7>,
                                KLib::RegisterPart<DLH, 0, 6>>  IERRegister;

        enum FCRRegisterPart
        {
            FIFO_EN,
            RX_FIFO_CLEAR,
            TX_FIFO_CLEAR,
            DMA_MODE,
            TX_FIFO_TRIG,
            RX_FIFO_TRIG
        };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress,
                                KLib::RegisterPart<FIFO_EN, 0>,
                                KLib::RegisterPart<RX_FIFO_CLEAR, 1>,
                                KLib::RegisterPart<TX_FIFO_CLEAR, 2>,
                                KLib::RegisterPart<DMA_MODE, 3>,
                                KLib::RegisterPart<TX_FIFO_TRIG, 4, 6>,
                                KLib::RegisterPart<RX_FIFO_TRIG, 6>>    FCRRegister;

        enum LCRRegiserPart
        {
            CHAR_LENGTH,
            NB_STOP,
            PARITY_EN,
            PARITY_TYPE1,
            PARITY_TYPE2,
            BREAK_EN,
            DIV_EN
        };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress
                                , KLib::RegisterPart<CHAR_LENGTH, 0, 2>
                                , KLib::RegisterPart<NB_STOP, 2>
                                , KLib::RegisterPart<PARITY_EN, 3>
                                , KLib::RegisterPart<PARITY_TYPE1, 4>
                                , KLib::RegisterPart<PARITY_TYPE2, 5>
                                , KLib::RegisterPart<BREAK_EN, 6>
                                , KLib::RegisterPart<DIV_EN, 7>> LCRRegiser;

        enum MCRRegisterPart
        {
            DTR,
            RTS,
            RI_STS_CH,
            CD_STS_CH,
            LOOPBACK_EN,
            XON_EN,
            TCR_TLR
        };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress
                                , KLib::RegisterPart<DTR, 0>
                                , KLib::RegisterPart<RTS, 1>
                                , KLib::RegisterPart<RI_STS_CH, 2>
                                , KLib::RegisterPart<CD_STS_CH, 3>
                                , KLib::RegisterPart<LOOPBACK_EN, 4>
                                , KLib::RegisterPart<XON_EN, 5>
                                , KLib::RegisterPart<TCR_TLR, 6>>  MCRRegister;

        enum LSRRegisterPart
        {
            RX_FIFO_E,
            RX_OE,
            RX_PE,
            RX_FE,
            RX_PI,
            TX_FIFO_E,
            TX_SR_E,
            RX_FIFO_STS
        };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress
                                , KLib::RegisterPart<RX_FIFO_E, 0>
                                , KLib::RegisterPart<RX_OE, 1>
                                , KLib::RegisterPart<RX_PE, 2>
                                , KLib::RegisterPart<RX_FE, 3>
                                , KLib::RegisterPart<RX_PI, 4>
                                , KLib::RegisterPart<TX_FIFO_E, 5>
                                , KLib::RegisterPart<TX_SR_E, 6>
                                , KLib::RegisterPart<RX_FIFO_STS, 7>>  LSRRegister;

        enum MSRRegisterPart
        {
            CTS_STS,
            DSR_STS,
            RI_STS,
            DCD_STS,
            NCTS_STS,
            NDSR_STS,
            NRI_STS,
            NCD_STS,
        };

        typedef KLib::Register<unsigned char,
                                UARTBaseAddress
                                , KLib::RegisterPart<CTS_STS, 0>
                                , KLib::RegisterPart<DSR_STS, 1>
                                , KLib::RegisterPart<RI_STS, 2>
                                , KLib::RegisterPart<DCD_STS, 3>
                                , KLib::RegisterPart<NCTS_STS, 4>
                                , KLib::RegisterPart<NDSR_STS, 5>
                                , KLib::RegisterPart<NRI_STS, 6>
                                , KLib::RegisterPart<NCD_STS, 7>>  MSRRegister;

        enum SPRRegisterPart
        {
            SPR_WORD
        };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress,
                                KLib::RegisterPart<SPR_WORD, 0, 8>>  SPRRegister;

        enum MDR1RegisterPart
        {
            MODE_SELECT,
            IR_SLEEP,
            SET_TXIR,
            SCT,
            SIP_MODE,
            FRAME_END_MODE,
        };
        typedef KLib::Register<unsigned char,
                                UARTBaseAddress
                                , KLib::RegisterPart<MODE_SELECT, 0, 3>
                                , KLib::RegisterPart<IR_SLEEP, 3>
                                , KLib::RegisterPart<SET_TXIR, 4>
                                , KLib::RegisterPart<SCT, 5>
                                , KLib::RegisterPart<SIP_MODE, 6>
                                , KLib::RegisterPart<FRAME_END_MODE, 7>> MDR1Regiser;
    }

    class UART : public KLib::Driver
    {
    public:
        UART(size_t clock, UARTBaseAddress addr);
        ~UART();

        void init(size_t baud);

        const char* getEscapeSequence() const
        {
            return "\r\n";
        }

        void device_write(const char* str, size_t n)
        {
            for (; n; --n)
            {
                putc(*str++);
            }
        }

        void device_read(char* buff, size_t n)
        {
            for (; n; --n)
            {
                *buff++ = getc();
            }
        }

        void putc(char c)
        {
            while (lsr_.getValue<detail::LSRRegisterPart::TX_FIFO_E, uint8_t>() == 0);
            data_.writeValue(c);
        }

        char getc ()
        {
            while (lsr_.getValue<detail::LSRRegisterPart::RX_FIFO_E, uint8_t>() == 0);
            return data_.getValue<detail::DataRegisterPart::RHR, uint8_t>();
        }

    private:
        size_t                                  clock_;
        UARTBaseAddress                         base_address_;
        detail::DataRegister                    data_;
        detail::IERRegister                     ier_;
        detail::FCRRegister                     fcr_;
        detail::LCRRegiser                      lcr_;
        detail::MCRRegister                     mcr_;
        detail::LSRRegister                     lsr_;
        detail::MSRRegister                     msr_;
        detail::SPRRegister                     spr_;
        detail::MDR1Regiser                     mdr1_;
    };

}

#endif /* UART_HPP_ */
