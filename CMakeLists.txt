cmake_minimum_required(VERSION 3.0)

project(bfc)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not especified, building as release.")
        message(STATUS "Use -DCMAKE_BUILD_TYPE=debug|release to set the build type.")
    set(CMAKE_BUILD_TYPE release)
endif(NOT CMAKE_BUILD_TYPE)

add_definitions(-std=c++14 -Wall -Wextra -pedantic)

if (${CMAKE_BUILD_TYPE} STREQUAL release)
        add_definitions(-O2 -s)
else (${CMAKE_BUILD_TYPE} STREQUAL release)
        add_definitions(-DBFC_DEBUG -ggdb -g3)
endif (${CMAKE_BUILD_TYPE} STREQUAL release)

set(BFC_SOURCE main.cpp compiller.h utils.h)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(BFC_PLATFORM_SOURCE
        ${CMAKE_SOURCE_DIR}/arch/linux_x86/compiller_linux.cpp
        ${CMAKE_SOURCE_DIR}/arch/linux_x86/templates.h)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(BFC_PLATFORM_SOURCE
        ${CMAKE_SOURCE_DIR}/arch/windows_x86/compiller_windows.cpp
        ${CMAKE_SOURCE_DIR}/arch/windows_x86/templates.h)
else()
    message(FATAL_ERROR "Your Platform does not supported: ${CMAKE_SYSTEM_NAME}")
endif()

add_executable( bfc ${BFC_SOURCE} ${BFC_PLATFORM_SOURCE})
