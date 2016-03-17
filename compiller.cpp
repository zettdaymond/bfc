#include "compiller.h"

#include <cstdio>
#include <cstdlib>
#include <stack>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <elf.h>

std::string asmTemplate {""};
std::vector<std::string> asmTemplates;

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string templateBegin =
        "; This is an automatically generated NASM assembly\n"
        "; listing by Brainfuck Compiller\n"
        "BITS 32\n"
        "GLOBAL _start\n"
        "SECTION .text\n"
        "_start:\n"
        "\t; source starts here\n";
std::string templateEnd =
        ";exit\n"
        "   mov eax, 1\n"
        "   mov ebx , 0x5D\n"
        "   int 0x80\n"
        "SECTION .data\n"
        "   DATAPTR times 1000 dd 0\n";

std::string incPointerTmp() {
    std::string tmp =
            ";inc pointer\n"
            "\tmov eax, DATAPTR\n"
            "\tinc eax\n"
            "\tmov DATAPTR, eax\n";
    return tmp;
}

std::string decPointerTmp() {
    std::string tmp =
            ";dec pointer\n"
            "\tmov eax, DATAPTR\n"
            "\tdec eax\n"
            "\tmov DATAPTR, eax\n";
    return tmp;
}

std::string incValueTmp() {
    std::string tmp =
            ";inc value\n"
            "\tmov eax, [DATAPTR]\n"
            "\tinc eax\n"
            "\tmov [DATAPTR], eax\n";
    return tmp;
}

std::string decValueTmp() {
    std::string tmp =
            ";dec value\n"
            "\tmov eax, [DATAPTR]\n"
            "\tdec eax\n"
            "\tmov [DATAPTR], eax\n";
    return tmp;
}

std::string getValueTmp() {
    std::string tmp =
            ";put char\n"
            "\tmov eax, 4\n"
            "\tmov ebx, 1\n"
            "\tmov ecx, DATAPTR\n"
            "\tmov edx, 1\n"
            "\tint 0x80\n";
    return tmp;
}

std::string getStartLoopTmp() {
    std::string tmp =
            ";open loop -'[' \n"
            "WHILE_NUM: \n"
            "   mov eax, [DATAPTR] \n"
            "   cmp eax, 0 \n"
            "   je END_WHILE_NUM \n";
    return tmp;
}

std::string getEndLoopTmp() {
    std::string tmp =
            ";close loop - ']'\n"
            "   jmp WHILE_NUM\n"
            "END_WHILE_NUM:\n";
    return tmp;
}

#define LOAD_ADDRESS 0x8048000

Elf32_Ehdr createElfHeader()
{
    Elf32_Ehdr elf{
        {'\x7f', 'E', 'L', 'F', '\x01','\x01','\x01',0,0,0,0,0,0,0,0,'\x10'}
    };
    elf.e_type = ET_EXEC; //Executeble file
    elf.e_machine = EM_386; //Type of arch
    elf.e_version = EV_CURRENT; //
    elf.e_phoff = sizeof (Elf32_Ehdr);
    elf.e_shoff = LOAD_ADDRESS; //TEMPORARY!!!
    elf.e_flags = 0;
    elf.e_ehsize = sizeof (Elf32_Ehdr);
    elf.e_phentsize = sizeof(Elf32_Phdr);
    elf.e_phnum = 2; //text + data segment

    //TEMPORARY
    elf.e_shentsize = 0;
    elf.e_shnum = 0;
    elf.e_shstrndx = 0;
    return elf;
}

Elf32_Phdr createProgramHeader() {
    Elf32_Phdr header;

    header.p_type = PT_LOAD; // Both, .text and .bss
    header.p_offset; //determine later
    header.p_vaddr;
    header.p_paddr = 0;
    header.p_filesz;
    header.p_memsz;
    header.p_flags = PF_R | PF_X;
    header.p_align = 0x1000;

    return header;
}

void compile(std::string src, std::string outPath)
{
    /* if src file exists
     * generate asembly file for it
     * then generate elf/exe file for it.
     *
     * does not support linker/object files. just simple executable creator.
     */

    Elf32_Ehdr elfHeader = createElfHeader();

    std::ofstream out(outPath, std::ios::binary);
    out.write((char*)&elfHeader, sizeof(elfHeader));

    auto textProgramHeader = createProgramHeader();
    out.write((char*)&textProgramHeader, sizeof(textProgramHeader));
    out.close();

}

void init()
{
    std::stringstream buffer;

    std::ifstream templateFile("template_linux.asm");
    buffer << templateFile.rdbuf();
    templateFile.close();
    asmTemplate = buffer.str();
    buffer.str(""); //clear buffer;

    std::ifstream cmdFile("command_linux.asm");
    buffer << cmdFile.rdbuf();
    templateFile.close();

    asmTemplates = split(buffer.str(), '/');
}

std::string assembly(std::string source)
{
    init();

    std::string cmds = "";

    unsigned lastID = 0;
    std::stack<unsigned> stack;

    for(auto token : source) {
        if(token == '>') {
            cmds.append( asmTemplates[0] );
            continue;
        }
        if(token == '<') {
            cmds.append( asmTemplates[1] );
            continue;
        }
        if (token == '+') {
            cmds.append( asmTemplates[2] );
            continue;
        }
        if (token == '-') {
            cmds.append( asmTemplates[3] );
            continue;
        }
        if (token == '.') {
            cmds.append( asmTemplates[4] );
            continue;
        }
        if (token == '[') {
            auto templ = asmTemplates[6];

            stack.push(lastID);
            replaceAll( templ, "NUM", std::to_string(lastID) );
            lastID++;

            cmds.append( templ );
            continue;
        }
        if (token == ']') {
            auto templ = asmTemplates[7];

            auto id = stack.top();
            stack.pop();
            replaceAll( templ, "NUM", std::to_string(id) );
            lastID++;

            cmds.append( templ );
            continue;
        }
        printf("Could not recognize token : '%c' \n", token);
    }


    if (stack.size() != 0) {
        printf(" Number of '[' does not equal to number of ']'");
        std::exit(255);
    }

    auto ret = asmTemplate;
    replaceAll(ret, "{SOURCE}", cmds);
    ret.append("\n");

    return ret;
}
