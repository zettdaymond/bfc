#ifndef BIN_TEMPLATES_H
#define BIN_TEMPLATES_H

#include <cstdlib>
#include <stddef.h>
#include <unordered_map>
#include <functional>

#include "utils.h"

std::string pointerIncBinTemplate(auto arg) {
    auto b = splitToBytes(arg * 4);
    return std::string {
        '\x81','\xC6', b[3], b[2], b[1], b[0]
    };
}

std::string pointerDecBinTemplate(auto arg) {
    auto b = splitToBytes(arg * 4);
    return std::string {
        '\x81','\xEE', b[3], b[2], b[1], b[0]
    };
}

std::string valueIncBinTemplate(auto arg) {
    auto b = splitToBytes(arg);
    return std::string {
        '\x81','\x06', b[3], b[2], b[1], b[0]
    };
}

std::string valueDecBinTemplate(auto arg) {
    auto b = splitToBytes(arg);
    return std::string {
        '\x81','\x2E', b[3], b[2], b[1], b[0]
    };
}

std::string startLoopBinTemplate(auto arg) {
    auto b = splitToBytes(arg);
    return std::string {
        '\x83','\x3E','\x00',
        '\x0F','\x84', b[3], b[2], b[1], b[0]
    };
}

std::string stopLoopBinTemplate(auto arg) {
    auto b = splitToBytes(arg);
    return std::string {
        '\xE9',b[3], b[2], b[1], b[0]
    };
}

std::string getcharBinTemplate(auto dummy_address, auto readfile_func_address) {
    auto dummy = splitToBytes(dummy_address);
    auto func_addr_bytes = splitToBytes(readfile_func_address);
    return std::string {
        //push NULL
        '\x6A','\x00',
        //push dummy
        '\x68', dummy[3], dummy[2], dummy[1], dummy[0],
        //push 1
        '\x6A','\x01',
        //push esi
        '\x56',
        //push ebx
        '\x53',
        //call _ReadFile@20
        '\xE8', func_addr_bytes[3], func_addr_bytes[2], func_addr_bytes[1], func_addr_bytes[0]
    };
}

std::string putcharBinTemplate(auto dummy_address, auto writefile_func_address) {
    auto dummy = splitToBytes(dummy_address);
    auto func_addr_bytes = splitToBytes(writefile_func_address);
    return std::string {
        //push NULL
        '\x6A','\x00',
        //push dummy
        '\x68', dummy[3], dummy[2], dummy[1], dummy[0],
        //push 1
        '\x6A','\x01',
        //push esi
        '\x56',
        //push ebx
        '\x53',
        //call _WriteFile@20
        '\xFF', '\x15', func_addr_bytes[3], func_addr_bytes[2], func_addr_bytes[1], func_addr_bytes[0]
    };
}

std::string preambuleBinTemplate(auto data_ptr, auto get_std_func_adress) {
    auto data = splitToBytes(data_ptr);
    auto func_ptr = splitToBytes(get_std_func_adress);
    return std::string {
        //mov esi, dataptr
        '\xBE', data[3], data[2], data[1], data[0],
        //push STD_OUTPUT_HANDLE
        '\x6A', '\xF5',
        //call _GetStdHandle@4
        '\xFF', '\x15', func_ptr[3], func_ptr[2], func_ptr[1], func_ptr[0],
        //mov ebx, eax
        '\x89', '\xC3'
    };
}

std::string postambuleBinTemplate(auto func_address) {
    auto func_address_bytes = splitToBytes(func_address);
    return std::string {
        //push 0
        '\x6A','\x00',
        //call _ExitProcess@4
        '\xFF', '\x15', func_address_bytes[3], func_address_bytes[2], func_address_bytes[1], func_address_bytes[0],
    };
}

#endif //BIN_TEMPLATES_H
