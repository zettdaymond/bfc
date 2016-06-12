cmake_minimum_required(VERSION 3.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(bfc)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not especified, building as release.")
    message(STATUS "Use -DCMAKE_BUILD_TYPE=Debug|Release to set the build type.")
    set(CMAKE_BUILD_TYPE release)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_definitions( -Wall -Wextra -pedantic)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug OR ${CMAKE_BUILD_TYPE} STREQUAL debug  OR ${CMAKE_BUILD_TYPE} STREQUAL Debug )
    add_definitions(-DBFC_DEBUG)
endif()

set(BFC_SOURCE main.cpp compiller.h utils.h utils.cpp compiller.cpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(BFC_PLATFORM_SOURCE
        ${CMAKE_SOURCE_DIR}/arch/linux_x86/compiller_linux.cpp
        ${CMAKE_SOURCE_DIR}/arch/linux_x86/asm_templates.h
        ${CMAKE_SOURCE_DIR}/arch/linux_x86/bin_templates.h)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#    message(FATAL_ERROR "Work for your platform is not complete.: ${CMAKE_SYSTEM_NAME}")
    set(BFC_PLATFORM_SOURCE
        ${CMAKE_SOURCE_DIR}/arch/windows_x86/compiller_windows.cpp
        ${CMAKE_SOURCE_DIR}/arch/windows_x86/asm_templates.h
        ${CMAKE_SOURCE_DIR}/arch/windows_x86/bin_templates.h
        ${CMAKE_SOURCE_DIR}/arch/windows_x86/exe_builder.cpp)
else()
    message(FATAL_ERROR "Your Platform does not supported: ${CMAKE_SYSTEM_NAME}")
endif()
add_executable( bfc ${BFC_SOURCE} ${BFC_PLATFORM_SOURCE})

set_property(TARGET bfc PROPERTY CXX_STANDARD 14)
