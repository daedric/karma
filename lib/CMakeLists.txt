INCLUDE_DIRECTORIES(include)

SET(KLIB_SOURCES
        src/logger/Logger.cpp
        src/logger/LoggerOverload.cpp
        src/logger/LoggerFormat.cpp
        )

ADD_LIBRARY(libk STATIC ${KLIB_SOURCES})
