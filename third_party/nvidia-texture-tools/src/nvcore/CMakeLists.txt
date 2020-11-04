PROJECT(nvcore)

SET(CORE_SRCS
    nvcore.h
    Array.h
    Debug.h Debug.cpp
    DefsGnucDarwin.h
    DefsGnucLinux.h
    DefsGnucWin32.h
    DefsVcWin32.h
    FileSystem.h FileSystem.cpp
    ForEach.h
    Library.h Library.cpp
    Memory.h Memory.cpp
    Ptr.h
    RefCounted.h
    StrLib.h StrLib.cpp
    Stream.h
    StdStream.h
    TextWriter.h TextWriter.cpp
    Timer.h Timer.cpp
    Utils.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# targets
ADD_DEFINITIONS(-DNVCORE_EXPORTS)

IF(UNIX)
    SET(LIBS ${LIBS} ${CMAKE_DL_LIBS})
ENDIF(UNIX)

IF(NVCORE_SHARED)
    ADD_DEFINITIONS(-DNVCORE_SHARED=1)
    ADD_LIBRARY(nvcore SHARED ${CORE_SRCS})
ELSE(NVCORE_SHARED)
    ADD_LIBRARY(nvcore ${CORE_SRCS})
ENDIF(NVCORE_SHARED)

TARGET_LINK_LIBRARIES(nvcore ${LIBS})

# On NetBSD and FreeBSD backtrace() is provided by libexecinfo, not libc.
if (CMAKE_SYSTEM_NAME MATCHES "NetBSD" OR CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
     TARGET_LINK_LIBRARIES(nvcore execinfo)
endif()

INSTALL(TARGETS nvcore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)
