cmake_minimum_required(VERSION 3.0)

project(bfc)

add_definitions(-std=c++14 -rdynamic -Wall -Wextra -g3 -O2)

file(GLOB_RECURSE bfc_SRCs
    "*.cpp"
    "*.h"
    "*.hpp"
)

add_executable( bfc ${bfc_SRCs} )
