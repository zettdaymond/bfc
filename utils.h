#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <array>
#include <string>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <algorithm>

#if defined(BFC_DEBUG)
#   define DUMP_VAR( V ) std::cout << (#V) << " : " << V << std::endl;
#   define DUMP_COLLECTION( V ) std::cout << (#V) << " : " << std::endl; dump( V );
#   define DEBUG_LAMBDA_TRAVERSE( C, L ) \
        std::cout << (#C) << " :" << std::endl; \
        std::for_each(C.begin(), C.end(), (L)); \
        std::cout << std::endl;
#   define DEBUG_PRINT( S ) std::cout << (S) << std::endl;
#else
#   define DUMP_VAR( V ) ((void)0);
#   define DUMP_COLLECTION( V ) ((void)0);
#   define DEBUG_LAMBDA_TRAVERSE( C, L ) ((void)0);
#   define DEBUG_PRINT( S ) ((void)0);
#endif

struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs);

inline HexCharStruct hex(unsigned char _c);

template<class T>
void dump( T& v) {
    std::cout << "Dump : ";
    for(auto c : v) {
        std::cout << hex(c)  << " ";
    }
    std::cout << "\t|\tSIZE: " << v.size() << std::endl << std::endl;
}

bool replace(std::string& str, const std::string& from, const std::string& to);

void replaceAll(std::string& str, const std::string& from, const std::string& to);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);

void pushBackArray(std::vector<char>& v, std::initializer_list<char> l);

std::array<char, 4> splitToBytes( unsigned n );

unsigned cast_to_U32(long int n);

std::pair<unsigned, unsigned> getLineAndOccurance(unsigned pos, const std::string& str);

#endif //UTILS_H
