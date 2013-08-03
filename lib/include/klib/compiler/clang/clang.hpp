#ifndef CLANG_HPP_
# define CLANG_HPP_

# if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
# define __section__(section_name)
# else
# define __section__ (section_name) __attribute__((section(section_name)))
# endif

# define __align__ (alignment) __attribute__((aligned(alignment)))


#endif /* COMPILER_HPP_ */
