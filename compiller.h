#include <string>

namespace Compiller 
{

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

/**
 * @brief Compiles source into binary file on running platform
 * @param source - Brainfuck instruction set
 * @return executable binary data
 */
std::string compile(const std::string& source);

/**
 * @brief translates Brainfuck source into NASM assembly.
 * @param source Brainfuck instruction set
 * @return NASM assembly instructions
 */
std::string assembly(const std::string& source);

}
