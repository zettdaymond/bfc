#include "../../compiller.h"
#include "../../utils.h"

#include <sstream>
#include <stack>

#include "asm_templates.h"
#include "bin_templates.h"

using namespace bfc;

std::string bfc::toAssembly(const std::vector<ByteCode> code) {
    std::string out = templateBegin;
    for (auto &op : code) {
        auto code = asm_templates[op.op];
        replaceAll(code, "ARG", std::to_string(op.arg));
        out += code;
    }
    out += templateEnd;
    return out;
}

std::string bfc::toBinnary(const std::vector<ByteCode> code) {
    std::string bin_out;
    std::stack<unsigned> stack;

    for (auto &op : code) {
        if (op.op == '[') {
            stack.push(bin_out.size());
        }
        bin_out += bin_templates[op.op](op.arg);
        if (op.op == ']') {
            //------------------Magick code starts...----------------------------
            //FIND JMP ADRESSES

            //FIRST: Find shifts to begin and end of the loop needed by je jmp
            //bin_out.size() contains shift that coresponds end of the loop

            //SECOND:
            //in the top of the 'stack' we store shift that corresponds to start of this loop
            auto loop_start = stack.top();
            stack.pop();

            //THIRD: Find shift between NEXT AFTER jmp instruction and begin of the loop
            // this is a negative number means how many bytes we need to jump BACK
            // relative to this position
            auto jmp_shift = loop_start - bin_out.size();

            //split by bytes and fill by little endian encoding rules
            auto jmp_shift_bytes = splitToBytes(jmp_shift);
            bin_out[bin_out.size() - 1] = jmp_shift_bytes[0];
            bin_out[bin_out.size() - 2] = jmp_shift_bytes[1];
            bin_out[bin_out.size() - 3] = jmp_shift_bytes[2];
            bin_out[bin_out.size() - 4] = jmp_shift_bytes[3];

            //FOURTH: Calculate the shift between NEXT AFTER the je instruction and the end of the loop
            // 9 corresponds number of bytes cmp instruction + je instruction itself.
            auto je_shift = bin_out.size() - ( loop_start + 9 );

            //split by bytes and fill by little endian encoding rules
            auto je_shift_bytes = splitToBytes(je_shift);

            //move forward to skip 3 cmp instruction bytes and 2 je opcode bytes
            loop_start += 5;

            bin_out[loop_start]     = je_shift_bytes[3];
            bin_out[loop_start + 1] = je_shift_bytes[2];
            bin_out[loop_start + 2] = je_shift_bytes[1];
            bin_out[loop_start + 3] = je_shift_bytes[0];
        }
    }

    bin_out += bin_end_template;

    auto bss_address = LOAD_ADDRESS + sizeof(BinaryHeader) + bin_out.size();
    bin_out.insert(0, bin_start_template(bss_address));

    //Create complete elf binnary data
    std::stringstream out;

    BinaryHeader header = createBinaryHeader( bin_out.size() );

    out.write((char*)&header, sizeof(header));
    out.write((char*)&bin_out[0], bin_out.size()* sizeof(char));

    return out.str();
}
