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
 *  Iterate over source string and try to distinguish brainfuck instructions.
 *  If symbol can not been recognized - prints error and returns empty instruction set.
 *
 * @param source - Raw source string.
 * @return Sequence of Brainfuck instructions.
 */
std::vector<char> lexAnalyse(const std::string& source);

/**
 * @brief Checks that set of tokens is conforming to the rules of a Brainfuck formal grammar.
 * @param Set of tokens
 * @return true, if instructions form valid Brainfuck program.
 */
bool syntaxAnalyse(const std::vector<char>& v);

/**
 * @brief Perform some checks like seeking infinite loops.
 * @param Set of tokens
 * @return true, if all checks passed.
 */
bool semanticAnalyse(const std::vector<char> &v);


/**
 * @brief Generates optimized internal byte code.
 * @param Valid set of tokens.
 * @return Insternal byte code.
 */
std::vector<ByteCode> optimize(const std::vector<char> &v);

/**
 * @brief Translates internal byte code representation into NASM assembly.
 * @param code - Brainfuck instruction set
 * @return NASM assembly instructions
 */
std::string toAssembly(const std::vector<ByteCode> code);

/**
 * @brief Translates internal byte code into binary file on running platform
 * @param code - Brainfuck instruction internal representation
 * @return executable binary data
 */
std::string toBinnary(const std::vector<ByteCode> code);

}
