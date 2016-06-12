#include "utils.h"

#include <vector>

bool replace(std::string& where, const std::string& what, const std::string& to) {
    size_t start_pos = where.find(what);
    if(start_pos == std::string::npos)
        return false;
    where.replace(start_pos, what.length(), to);
    return true;
}

void replaceAll(std::string& where, const std::string& what, const std::string& to) {
    if(what.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = where.find(what, start_pos)) != std::string::npos) {
        where.replace(start_pos, what.length(), to);
        start_pos += to.length(); // In case 'to' contains 'what', like replacing 'x' with 'yx'
    }
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

std::pair<unsigned, unsigned> getLineAndOccurrence(unsigned pos, const std::string& str) {
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

void writeWithAlign(std::ostream &out, char *data, unsigned size, unsigned align)
{
    auto bytes_to_align = (align - (size % align));
    std::vector<char> padd;
    padd.resize(bytes_to_align);
    for (auto& c : padd) {
        c = 0;
    }

    out.write(data, size);
    out.write(&padd[0], padd.size() * sizeof(char));
}
