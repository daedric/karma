#ifndef NONCOPYABLE_HPP_
# define NONCOPYABLE_HPP_

namespace KLib
{
struct NonCopyable
{
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

protected:
    NonCopyable(){}
    ~NonCopyable(){}

};
}

#endif /* NONCOPYABLE_HPP_ */
