#include <string>

struct CompillerState
{
    std::string outFile = "a.out";
    std::string src;
    bool assembly = false;
};



void compile(std::__cxx11::string src, std::__cxx11::string out);

std::string assembly(std::string source);
