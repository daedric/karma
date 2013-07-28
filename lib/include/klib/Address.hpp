#ifndef ADDRESS_HPP_
# define ADDRESS_HPP_

# include "Types.h"
# include "Traits.hpp"

namespace KLib
{

    template<size_t PointedSizeInBytes = 4, bool Writeable = true, bool Volatile = false, bool Readable = true>
    class Address
    {

    public:

        template<typename T = void>
        struct GetPointerType
        {
            typedef typename Traits::static_if_c<Readable, T, void>::type pass0;
            typedef typename Traits::static_if_c<Volatile, typename Traits::add_volatile<pass0>::type, pass0>::type pass1;
            typedef typename Traits::static_if_c<Writeable, pass1, typename Traits::add_const<pass1>::type>::type pass2;
            typedef typename Traits::add_pointer<pass2>::type pass3;
            using type = pass3;
        };

        static const size_t PointedSize = PointedSizeInBytes;
        static const bool isWritable = Writeable;
        static const bool isVolatile = Volatile;

        template <typename T>
        Address(T* pointer)
        : address_(size_t(pointer))
        {
            static_assert(sizeof(T) == PointedSize, "Incompatible pointer type");
        }

        Address(void* pointer)
        : address_(size_t(pointer))
        {
            static_assert(PointedSize == 0, "Pointed size must be 0 when working with void*");
        }


        Address(size_t address)
                : address_(address)
        {

        }

        Address(const Address& rhs)
                : address_(rhs.address_)
        {

        }

        Address(Address&& addr)
        : address_(addr.address_)
        {
            addr.address_ = 0;
        }

        template <bool _Writeable, bool _Volatile, bool _Readable>
        operator Address<PointedSizeInBytes, _Writeable, _Volatile, _Readable> ()
        {
            return Address<PointedSizeInBytes, _Writeable, _Volatile, _Readable>(address_);
        }

        Address& operator=(size_t address)
        {
            this->address_ = address;
            return *this;
        }

        Address& operator+=(size_t offset)
        {
            this->address_ += offset;
            return *this;
        }

        Address operator+(const Address& rhs) const
        {
            Address addr(*this);
            addr += rhs.address_;
            return addr;
        }


        template <size_t AlignedOn>
        bool isAligned() const
        {
            return (address_ & (1 << AlignedOn) - 1) == 0;
        }

        template<typename T>
        typename GetPointerType<T>::type getAddress()
        {
            static_assert((PointedSizeInBytes == 0) || (sizeof(T) == PointedSizeInBytes), "Cannot cast the pointer, invalid size");
            return reinterpret_cast<typename GetPointerType<T>::type>(address_);
        }

    private:
        size_t address_;
    };
}

#endif /* ADDRESS_HPP_ */
