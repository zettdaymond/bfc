#include <string>
#include <vector>

namespace bfc
{

struct CompillerState
{
    std::string outFile = "a.out";
    std::string src;
    bool assembly = false;
};

struct ByteCode {
    char op;
    unsigned arg;
};

/**
 * @brief Converts string into a sequence of Brainfuck instructions.
 *  Iterates over source string and tries to recognize brainfuck instructions.
 *  If symbol can not be recognized, prints error and returns empty instruction sequence.
 *
 * @param source - Raw source string.
 * @return Sequence of Brainfuck instructions.
 */
std::vector<char> lexAnalysis(const std::string& source);

/**
 * @brief Checks that sequence of tokens is conforming to the rules of a Brainfuck formal grammar.
 * @param Sequence of tokens
 * @return true, if instructions form valid Brainfuck program.
 */
bool syntaxAnalysis(const std::vector<char>& v);

/**
 * @brief Perform some checks like detecting infinite loops.
 * @param Sequence of tokens
 * @return true, if all checks passed.
 */
bool semanticAnalysis(const std::vector<char> &v);


/**
 * @brief Generates optimized internal byte code.
 * @param Valid sequence of tokens.
 * @return Internal byte code.
 */
std::vector<ByteCode> optimize(const std::vector<char> &v);

/**
 * @brief Translates internal byte code representation into NASM assembly.
 * @param code - Brainfuck instruction sequence
 * @return NASM assembly instructions
 */
std::string toAssembly(const std::vector<ByteCode> &code);

/**
 * @brief Translates internal byte code into binary file on targetplatform
 * @param code - Brainfuck instruction internal representation
 * @return executable binary data
 */
std::string toBinary(const std::vector<ByteCode> &code);

}
