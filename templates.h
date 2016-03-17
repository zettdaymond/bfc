#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <string>

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

#endif // TEMPLATES_H
