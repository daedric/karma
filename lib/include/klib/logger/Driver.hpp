/*
 * Driver.hpp
 *
 *  Created on: May 5, 2013
 *      Author: daedric
 */

#ifndef DRIVER_HPP_
# define DRIVER_HPP_

# include "klib/String.hpp"

namespace KLib
{

class Driver
{
public:

    enum eCapabilities
    {
        CAN_READ  = (1 << 0),
        CAN_WRITE = (1 << 1),
        NOTHING   = 0,
    };

    Driver()
    : capabilities_(NOTHING)
    , escape_sequence_size_(0)
    {}

    Driver(int mask)
    : capabilities_(mask)
    , escape_sequence_size_(0)
    {}


    virtual ~Driver(){}

    virtual const char* getEscapeSequence() const
    {
        return "\n";
    }

    void writeEscapeSequence()
    {
        if (escape_sequence_size_ == 0)
        {
            escape_sequence_size_ = kstrlen(getEscapeSequence());
        }

        write(getEscapeSequence(), escape_sequence_size_);
    }

    void write(const char* data, size_t size)
    {
        if (capabilities_ & CAN_WRITE)
        {
            device_write(data, size);
        }
    }

    void read(char* data, size_t size)
    {
        if (capabilities_ & CAN_READ)
        {
            device_read(data, size);
        }
    }

    static void endl(Driver& dev)
    {
        dev.writeEscapeSequence();
    }

protected:
    virtual void device_write(const char*, size_t) {};
    virtual void device_read(char*, size_t) {};

private:
    size_t          capabilities_;
    size_t          escape_sequence_size_;
};


}


#endif /* DRIVER_HPP_ */
