/*
 * compiler.hpp
 *
 *  Created on: Aug 2, 2013
 *      Author: daedric
 */

#ifndef COMPILER_HPP_
# define COMPILER_HPP_

# if defined(__clang__)
#  include "clang/clang.hpp"
# endif

# if defined(__GNUC__)
#  include "gcc/gcc.hpp"
# endif

#endif /* COMPILER_HPP_ */
