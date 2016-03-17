#include <string>

struct CompillerState
{
    std::string outFile = "a.out";
    std::string src;
    bool assembly = false;
};

enum Operations
{
    INC_PTR = 0,
    DEC_PTR = 1,
    INC_VALUE = 2,
    DEC_VALUE = 3,
    PUT_CHAR = 4,
    GET_CHAR = 5,
    START_LOOP = 6,
    END_LOOP = 7,
    COUNT
};

void compile(std::string source, std::string out);

std::string assembly(std::string source);
