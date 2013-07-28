#ifndef __TYPES_H__
# define __TYPES_H__

typedef __INT8_TYPE__ int8_t;
typedef __UINT8_TYPE__ uint8_t;

typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;

typedef __INT32_TYPE__  int32_t;
typedef __UINT32_TYPE__ uint32_t;

typedef __SIZE_TYPE__ size_t;


constexpr size_t operator "" _Ko(unsigned long long size)
{
    return size * 1024;
}

constexpr size_t operator "" _Mo(unsigned long long size)
{
    return size * 1024_Ko;
}

constexpr size_t operator "" _Go(unsigned long long size)
{
    return size * 1024_Mo;
}

#endif
