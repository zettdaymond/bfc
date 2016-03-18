#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <array>
#include <string>
#include <initializer_list>

#if defined(BFC_DEBUG)
#   define DUMP_VAR( V ) std::cout << (#V) << " : " << V << std::endl;
#   define DUMP_COLLECTION( V ) std::cout << (#V) << " : " << std::endl; dump( V );
#else
#   define DUMP_VAR( V ) ((void)0);
#   define DUMP_COLLECTION( V ) ((void)0);
#endif

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

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
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

template<class T>
void dump( T& v) {
    std::cout << "Dump : ";
    for(auto c : v) {
        std::cout << hex(c)  << " ";
    }
    std::cout << "\t|\tSIZE: " << v.size() << std::endl << std::endl;
}

unsigned cast_to_U32(long int n) {
    return static_cast<uint32_t> (n);
}

#endif //UTILS_H
