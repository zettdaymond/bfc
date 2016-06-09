#include "compiller.h"

#include <sstream>
#include <stack>

#include "asm_templates.h"
#include "utils.h"

using namespace bfc;

std::string bfc::toAssembly(const std::vector<ByteCode> &code) {
    std::string out = templateBegin;
    for (auto &op : code) {
        auto code = asm_templates[op.op];
        replaceAll(code, "ARG", std::to_string(op.arg));
        out += code;
    }
    out += templateEnd;
    return out;
}

std::string bfc::toBinary(const std::vector<ByteCode> &code) {
    return std::string();
}
