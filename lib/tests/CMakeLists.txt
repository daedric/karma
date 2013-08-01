PROJECT(KLIB CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES(../include)

SET(KLIB_SOURCES
        ../src/logger/Logger.cpp
        ../src/logger/LoggerOverload.cpp
        ../src/logger/LoggerFormat.cpp
        )

ADD_LIBRARY(libk STATIC ${KLIB_SOURCES})


ENABLE_TESTING()

MACRO(ADD_KLIB_TEST MODULE TEST_NAME)
    SET(EXE_NAME Test_${TEST_NAME})
    ADD_EXECUTABLE(${EXE_NAME} ${TEST_NAME}.cpp)
    TARGET_LINK_LIBRARIES(${EXE_NAME} libk)
    ADD_TEST(${MODULE}_${TEST_NAME} ${EXE_NAME})
    SET_TESTS_PROPERTIES(${MODULE}_${TEST_NAME} PROPERTIES
                         FAIL_REGULAR_EXPRESSION "FAILED")
ENDMACRO(ADD_KLIB_TEST TEST_NAME)

ADD_DEFINITIONS(-W
                -Wall
                -O2
                -Wextra
                -std=c++11 
                -fno-exceptions
                -fno-rtti
                -fno-stack-protector)

INCLUDE_DIRECTORIES("${KLIB_SOURCE_DIR}/../include")
ADD_SUBDIRECTORY(RegisterTests)
ADD_SUBDIRECTORY(TypesTests)
ADD_SUBDIRECTORY(TraitsTests)
ADD_SUBDIRECTORY(LoggerTests)
ADD_SUBDIRECTORY(MemoryTests)