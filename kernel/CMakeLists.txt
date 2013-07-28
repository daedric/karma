INCLUDE_DIRECTORIES("${BOARD_PATH}/include")
INCLUDE_DIRECTORIES("${CURRENT_PROC_PATH}/include")
INCLUDE_DIRECTORIES("${KERNEL_LIBRARY_DIR}/include")
INCLUDE_DIRECTORIES("${CURRENT_ARCH_PATH}/common/include")
INCLUDE_DIRECTORIES("include")

SET(KernelSources
    src/CXXABI.cpp
    src/main.cpp)

SET_SOURCE_FILES_PROPERTIES(
    src/main.c PROPERTIES OBJECT_DEPENDS "${ARCH_CONFIG_PATH}/linker.ld"
)

SET(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_GENERATED} -nostartfiles -nodefaultlibs -nostdlib -Wl,-Map=Kernel.map ")
ADD_EXECUTABLE(Kernel.elf ${KernelSources})
TARGET_LINK_LIBRARIES(Kernel.elf libboard libcpu libk libgcc.a)

FIND_FILE(MKIMAGE_EXE
            NAMES mkimage
            PATHS $ENV{MKIMAGE_DIR})

IF (MKIMAGE_EXE)
    MESSAGE(STATUS "mkimage found")
    ADD_CUSTOM_COMMAND(TARGET Kernel.elf
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E echo "Building Kernel.bin"
                        COMMAND ${COMPILER_PREFIX}objcopy Kernel.elf -O binary Kernel.bin --image-base ${ENTRYPOINT_ADDRESS}
                        COMMAND ${CMAKE_COMMAND} -E echo "Building uImage"
                        COMMAND ${MKIMAGE_EXE} -A arm -n Karma -C none -O linux -T kernel -d Kernel.bin -a ${LOAD_ADDRESS} -e ${ENTRYPOINT_ADDRESS} -n karma uImage 
                        DEPENDS Kernel.elf
                        VERBATIM)
ELSE()
    MESSAGE(STATUS "mkimage not found")
    ADD_CUSTOM_COMMAND(TARGET Kernel.elf
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E echo "Building Kernel.bin"
                        COMMAND ${COMPILER_PREFIX}objcopy Kernel.elf -O binary Kernel.bin
                        DEPENDS Kernel.elf
                        VERBATIM)
ENDIF()
