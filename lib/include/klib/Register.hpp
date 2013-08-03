#ifndef __REGISTER_HPP__
# define __REGISTER_HPP__

# include "Types.h"
# include "Utils.hpp"
# include "Traits.hpp"
# include "NonCopyable.hpp"

namespace KLib
{

    template<size_t ID, size_t OFFSET_START, size_t OFFSET_END = OFFSET_START + 1>
    struct RegisterPart
    {
        static constexpr size_t RegisterID = ID;
        static_assert(OFFSET_END > OFFSET_START, "OFFSET_START > OFFSET_END");

        template<typename ReturnType, typename T>
        static constexpr ReturnType getValue(T reg)
        {
            static_assert(OFFSET_START < getSizeInBits(T()), "OFFSET_START > reg");
            static_assert(OFFSET_END <= (getSizeInBits(T())), "OFFSET_END > reg");
            static_assert(((OFFSET_END - OFFSET_START) / 8) <= sizeof(ReturnType), "Invalid return type");
            return (ReturnType)sliceByte(reg, OFFSET_START, OFFSET_END);
        }

        static constexpr unsigned int MASK = ((1 << (OFFSET_END - OFFSET_START)) - 1) << OFFSET_START;

        template <typename T, typename U>
        static void setValue(T reg_addr, U value)
        {
            static_assert(Traits::is_const<typename Traits::remove_pointer<T>::type>::value == false, "The pointer is const, the register is not writable");
            auto old_value = *reg_addr;
            *reg_addr = old_value ^ ((old_value ^ (value << OFFSET_START)) & MASK);
        }

        template <typename T, typename U>
        static void setSlicedValue(T reg_addr, U value)
        {
            value = sliceByte(value, OFFSET_START, OFFSET_END);
            setValue(reg_addr, value);
        }

    };

    namespace detail
    {
        template <size_t idx, size_t S, size_t E>
        static constexpr RegisterPart<idx, S, E>& get(RegisterPart<idx, S, E>& ref)
        {
            return ref;
        }

        template<typename RegisterUnderlyingType, typename... RegisterValues>
        struct Register : public RegisterValues...
        {
            template <size_t RegisterValueIdx, typename ReturnType, typename T>
            ReturnType getValue(T value)
            {
                return detail::get<RegisterValueIdx>(*this).template getValue<ReturnType>(value);
            }

            template <size_t RegisterValueIdx, typename T, typename U>
            void setValue(T reg, U value)
            {
                detail::get<RegisterValueIdx>(*this).setValue(reg, value);
            }

            template <size_t RegisterValueIdx, typename T, typename U>
            void setSlicedValue(T reg, U value)
            {
                    detail::get<RegisterValueIdx>(*this).setSlicedValue(reg, value);
            }

        };

    }

    // the code could be improved by using a Tuple-like
    // but I don't want to recode it for now.
    template<typename RegisterUnderlyingType, typename Address, typename... RegisterValues>
    struct Register : private detail::Register<RegisterUnderlyingType, RegisterValues...>
                    , private NonCopyable
    {

    private:

        typedef detail::Register<RegisterUnderlyingType, RegisterValues...> Base;
        typedef typename Address::template GetPointerType<RegisterUnderlyingType>::type PointerType;

        struct RegisterHolderRAII
        {

            RegisterHolderRAII(Base& reg, PointerType ptr)
            : reg_(reg)
            , real_pointer_(ptr)
            , cached_value_(*ptr)
            {
            }

            RegisterHolderRAII(RegisterHolderRAII&& rv)
            : reg_(rv.reg_)
            , real_pointer_(rv.real_pointer_)
            , cached_value_(rv.cached_value_)
            {}

            RegisterHolderRAII(const RegisterHolderRAII& rv) = delete;

            ~RegisterHolderRAII()
            {
                *real_pointer_ = cached_value_;
            }

            template <size_t RegisterValueIdx, typename U>
            RegisterHolderRAII& setValue(U value)
            {
                reg_.template setValue<RegisterValueIdx>(&cached_value_, value);
                return *this;
            }

            template <size_t RegisterValueIdx, typename U>
            RegisterHolderRAII& setSlicedValue(U value)
            {
                reg_.template setSlicedValue<RegisterValueIdx>(&cached_value_, value);
                return *this;
            }


        private:
            Base&           reg_;
            PointerType     real_pointer_;
            typedef typename Traits::remove_pointer<PointerType>::type ValueType;
            ValueType       cached_value_;
        };

    public:
        typedef Address AddressType;

        Register(Address addr)
        : address_(addr)
        {}

        template <size_t RegisterValueIdx, typename ReturnType>
        ReturnType getValue()
        {
            return Base::template getValue<RegisterValueIdx, ReturnType>(*address_.template getAddress<RegisterUnderlyingType>());
        }

        template <size_t RegisterValueIdx, typename U>
        RegisterHolderRAII setValue(U value)
        {
            RegisterHolderRAII ret(*this, address_.template getAddress<RegisterUnderlyingType>());
            ret.template setValue<RegisterValueIdx>(value);
            return ret;
        }

        template <size_t RegisterValueIdx, typename U>
        RegisterHolderRAII setSlicedValue(U value)
        {
            RegisterHolderRAII ret(*this, address_.template getAddress<RegisterUnderlyingType>());
            ret.template setSlicedValue<RegisterValueIdx>(value);
            return ret;
        }

        void writeValue(RegisterUnderlyingType value)
        {
            static_assert(Traits::is_const<typename Traits::remove_pointer<PointerType>::type>::value == false, "The pointer is const, the register is not writable");
            auto reg_addr = address_.template getAddress<RegisterUnderlyingType>();
            *reg_addr = value;
        }

    private:
        Address address_;
    };
}

# include "Address.hpp"
namespace KLib
{

template<typename ... RegisterValues>
using Default32bRegister = Register<unsigned int, Address<4>, RegisterValues...>;

template<typename ... RegisterValues>
using Default8bRegister = Register<unsigned char, Address<1>, RegisterValues...>;

template<typename ... RegisterValues>
using Volatile32bRegister = Register<unsigned int, Address<4, true, true>, RegisterValues...>;

template<typename ... RegisterValues>
using Volatile8bRegister = Register<unsigned char, Address<1, true, true>, RegisterValues...>;

}
#endif /* REGISTER_HPP_ */
