#include "compiller.h"

#include <sstream>
#include <stack>

#include "asm_templates.h"
#include "exe_builder.h"
#include "bin_templates.h"
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
    std::string bin_out;
    std::string code_section;
    std::stack<unsigned> stack;

    //STEP 1: compute size of the code.
    unsigned raw_code_size = 0;
    raw_code_size += preambuleBinTemplate(0,0).size();
    for (auto &op : code) {
        switch(op.op) {
        case '>':
            raw_code_size += pointerIncBinTemplate(0).size();
            break;
        case '<':
            raw_code_size += pointerDecBinTemplate(0).size();
            break;
        case '+':
            raw_code_size += valueIncBinTemplate(0).size();
            break;
        case '-':
            raw_code_size += valueDecBinTemplate(0).size();
            break;
        case '[':
            raw_code_size += startLoopBinTemplate(0).size();
            break;
        case ']':
            raw_code_size += stopLoopBinTemplate(0).size();
            break;
        case ',':
            raw_code_size += getcharBinTemplate(0,0).size();
            break;
        case '.':
            raw_code_size += putcharBinTemplate(0,0).size();
            break;
        default:
            //assert()
            break;
        }
    }
    raw_code_size += postambuleBinTemplate(0).size();



    const unsigned rounded_to_section_code_size = roundToAlign(raw_code_size, SECTION_ALIGN);
    const unsigned rounded_to_file_code_size = roundToAlign(raw_code_size, FILE_ALIGN);

    //dummy starts in the begining of bss segmrnt. immidiatly after Header + .code sections.
    const unsigned dummy_address = IMAGEBASE + HEADER_TO_SECTION_ROUNDED_SIZE + rounded_to_section_code_size;
    //data starts immidiatly after dummy var.
    const unsigned data_address = dummy_address + sizeof(dummy_address);

    BinaryHeader header = createBinaryHeader(raw_code_size);
//    const unsigned int header_rounded_to_file_size = roundToAlign(sizeof(header), SECTION_ALIGN);
    ImportSection import_section = createImportsSection(raw_code_size);
    auto kernel_function_adresses = getKernelFunctionAddresses(raw_code_size);

    bin_out += preambuleBinTemplate(data_address,kernel_function_adresses.get_std_handle);

    //codogeneration process.
    for (auto &op : code) {
        if (op.op == '[') {
            stack.push(bin_out.size());
            bin_out += startLoopBinTemplate(op.arg);
        }
        if (op.op == '>') {
            bin_out += pointerIncBinTemplate(op.arg);
        }
        if (op.op == '<') {
            bin_out += pointerDecBinTemplate(op.arg);
        }
        if (op.op == '+') {
            bin_out += valueIncBinTemplate(op.arg);
        }
        if (op.op == '-') {
            bin_out += valueDecBinTemplate(op.arg);
        }
        if(op.op == '.') {
            bin_out += putcharBinTemplate(dummy_address, kernel_function_adresses.write_file);
        }
        if(op.op == ',') {
            bin_out += getcharBinTemplate(dummy_address, kernel_function_adresses.read_file);
        }
        if (op.op == ']') {
            bin_out += stopLoopBinTemplate(op.arg);
            //------------------Magick code starts...----------------------------
            auto loop_start = stack.top();
            stack.pop();

            auto jmp_shift = loop_start - bin_out.size();

            //split by bytes and fill by little endian encoding rules
            auto jmp_shift_bytes = splitToBytes(jmp_shift);
            bin_out[bin_out.size() - 1] = jmp_shift_bytes[0];
            bin_out[bin_out.size() - 2] = jmp_shift_bytes[1];
            bin_out[bin_out.size() - 3] = jmp_shift_bytes[2];
            bin_out[bin_out.size() - 4] = jmp_shift_bytes[3];

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

    bin_out += postambuleBinTemplate(kernel_function_adresses.exit_process);

    //write to file;
    std::stringstream out;

    writeWithAlign(out, (char*)&header, sizeof(header), FILE_ALIGN);
    writeWithAlign(out, (char*)&bin_out[0], bin_out.size()* sizeof(char), FILE_ALIGN);
    writeWithAlign(out, (char*)&import_section, sizeof(import_section), FILE_ALIGN);

    return out.str();
}
