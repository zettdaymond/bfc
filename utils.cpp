#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <array>
#include <string>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void pushBackArray(std::vector<char>& v, std::initializer_list<char> l)
{
    for(auto e : l) {
        v.push_back( e );
    }
}

struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

HexCharStruct hex(unsigned char _c)
{
  return HexCharStruct(_c);
}

std::array<char, 4> splitToBytes( unsigned n )
{
    std::array<char, 4> bytes;

    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    return bytes;
}

unsigned cast_to_U32(long int n) {
    return static_cast<uint32_t> (n);
}

std::pair<unsigned, unsigned> getLineAndOccurance(unsigned pos, const std::string& str) {
    auto n{0u};
    auto occ{0u};
    if (pos >= str.length()) {
        return std::make_pair(-1, -1);
    }
    for(auto i{0u}; i <= pos; i++) {
        occ++;
        if (str[i] == '\n') {
            n++;
            occ = 0;
        }
    }
    return std::make_pair(n + 1, occ);
}

#endif //UTILS_H
