#ifndef UTILS_H
#define UTILS_H

#include <array>
#include <string>
#include <iostream>
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
inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) {
    return (o << std::hex << (int)hs.c);
}
inline HexCharStruct hex(unsigned char _c) {
    return HexCharStruct(_c);
}

/**
 * @brief Prints hexadecimal content of collection.
 */
template<class T>
void dump( T& v) {
    std::cout << "Dump : ";
    for(auto c : v) {
        std::cout << hex(c)  << " ";
    }
    std::cout << "\t|\tSIZE: " << v.size() << std::endl << std::endl;
}

/**
 * @brief Replaces one occurrence of substring to chosen string.
 * @param where - string, which be handled.
 * @param what - substring to replace.
 * @param to - new substring.
 * @return true, if success.
 */
bool replace(std::string& where, const std::string& what, const std::string& to);

/**
 * @brief Replaces all occurrences of substring to chosen string.
 * @param where - string, which be handled.
 * @param what - substring to replace.
 * @param to - new substring.
 */
void replaceAll(std::string& where, const std::string& what, const std::string& to);

/**
 * @brief Split one Double Word into 4 Bytes that represent it.
 * @param n - number, which be handled
 * @return array of splitted bytes in big-endian encoding.
 */
std::array<char, 4> splitToBytes( unsigned n );

/**
 * @brief Counts line-ending symbols between begining of the string and some position,
 *        and determine occurrence of this position in last line.
 * @param pos - position in string
 * @param str - string, which be handled
 * @return pair of lines count number and position in last line.
 */
std::pair<unsigned, unsigned> getLineAndOccurrence(unsigned pos, const std::string& str);

/**
 * @brief Writes data to output stream, and fills the rest by 0 according to the alingment.
 * @param out - output stream
 * @param data - pointer to binary data
 * @param size - count of bytes in data
 * @param align - alignment
 */
void writeWithAlign(std::ostream& out, char* data, unsigned size, unsigned align);

/**
 * @brief Rounds some value to chosen alignment.
 */
template<class T, class U>
T roundToAlign( T v, U align) {
    return v + (align - (v % align));
}

#endif //UTILS_H
