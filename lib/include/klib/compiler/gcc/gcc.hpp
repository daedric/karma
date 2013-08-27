#ifndef GCC_HPP_
# define GCC_HPP_

# if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
# define __section__(section_name)
# else
# define __section__(section_name) __attribute__((section(section_name)))
# endif

# define __align__(alignement) __attribute__((aligned(alignement)))
# define __packed__ __attribute__((packed))

#endif /* GCC_HPP_ */
