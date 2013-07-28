INCLUDE(CMakeForceCompiler)

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
SET(ARCH      "arm"       CACHE STRINGS "Targeted arch")
SET(PROCESSOR "armv7a"    CACHE STRINGS "Targeted processor")
SET(BOARD     "pandaes"   CACHE STRINGS "Targeted board")
SET(RAM_START_ADDRESS    "0x80000000" CACHE STRINGS "Kernel load address to build an u-boot image")
SET(LOAD_ADDRESS         "0x82000000" CACHE STRINGS "Kernel load address to build an u-boot image")
SET(ENTRYPOINT_ADDRESS   "0x82000040" CACHE STRINGS "Kernel entry point to build an u-boot image")

# specify the cross compiler
CMAKE_FORCE_C_COMPILER("${COMPILER_PREFIX}gcc" GNU)
CMAKE_FORCE_CXX_COMPILER("${COMPILER_PREFIX}g++" GNU)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

ADD_DEFINITIONS(-mcpu=cortex-a9 -march=armv7-a -mfpu=neon)

ADD_DEFINITIONS(-W
                -Wall
                -O2
                -fomit-frame-pointer
                -Wextra
                -nostdlib
                -fno-builtin
                -nostartfiles
                -nodefaultlibs
                -std=c++11 
                -Wzero-as-null-pointer-constant
                -fno-exceptions
                -fno-rtti
                -fno-stack-protector
                -nostdinc++
                -ffreestanding
                )
