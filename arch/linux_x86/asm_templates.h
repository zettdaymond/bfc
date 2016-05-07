#ifndef ASM_TEMPLATES_H
#define ASM_TEMPLATES_H

#include <string>
#include <unordered_map>

std::string templateBegin =
        "; This is an automatically generated NASM assembly\n"
        "; listing by Brainfuck Compiller\n"
        "BITS 32\n"
        "GLOBAL _start\n"
        "SECTION .bss\n"
        "\tDATAPTR: resd 30000\n"
        "SECTION .text\n"
        "_start:\n"
        "\tmov esi, DATAPTR\n";

std::string templateEnd =
        ";exit\n"
        "\tmov eax, 1\n"
        "\tmov ebx , 0x5D\n"
        "\tint 0x80\n";

static std::unordered_map<char, std::string> asm_templates = {
    {
        '>' ,
        ";inc pointer\n"
        "\tadd esi, 4 * ARG\n"
    },
    {
        '<',
        ";dec pointer\n"
        "\tsub esi, 4 * ARG\n"
    },
    {
        '+',
        ";inc value\n"
        "\tadd dword [esi], ARG\n"
    },
    {
        '-',
        ";dec value\n"
        "\tsub dword [esi], ARG\n"
    },
    {
        '.',
        ";putchar\n"
        "\tmov eax, 4\n"
        "\tmov ebx, 1\n"
        "\tmov ecx, esi\n"
        "\tmov edx, 1\n"
        "\tint 0x80\n"
    },
    {
        ',',
        ";getchar\n"
        "\tmov eax, 3\n"
        "\tmov ebx, 1\n"
        "\tmov ecx, esi\n"
        "\tmov edx, 1\n"
        "\tint 0x80\n"
    },
    {
        '[',
        ";start loop\n"
        "w_ARG:\n"
        "\tcmp dword [esi], 0\n"
        "\tje near ew_ARG\n"
    },
    {
        ']',
        ";stop loop\n"
        "\tjmp near w_ARG\n"
        "ew_ARG:\n"
    }
};

#endif // ASM_TEMPLATES_H
