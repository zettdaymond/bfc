#ifndef ASM_TEMPLATES_H
#define ASM_TEMPLATES_H

#include <string>
#include <unordered_map>

std::string templateBegin =
        "; This is an automatically generated NASM assembler\n"
        "; listing by Brainfuck Compiller\n\n"

        "STD_OUTPUT_HANDLE equ -11\n"
        "%define NULL    dword 0\n\n"

        "extern  _GetStdHandle@4\n"
        "extern  _WriteFile@20\n"
        "extern  _ReadFile@20\n"
        "extern  _ExitProcess@4\n\n"

        "BITS 32\n"
        "GLOBAL _start\n\n"

        "SECTION .text\n"
        "_start:\n"
        "    mov esi, dataptr\n"
        "; hStdOut = GetstdHandle(STD_OUTPUT_HANDLE)\n"
        "    push STD_OUTPUT_HANDLE\n"
        "    call _GetStdHandle@4\n"
        "    mov ebx, eax\n";

std::string templateEnd =
        "; ExitProcess(0)\n"
        "    push    0\n"
        "    call    _ExitProcess@4\n"
        "SECTION .bss\n"
        "    dummy: resd 1\n"
        "    dataptr: resd 30000\n";

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
        "push NULL\n"
        "push dummy\n"
        "push 1\n"
        "push esi\n"
        "push ebx\n"
        "call _WriteFile@20\n"
    },
    {
        ',',
        ";getchar\n"
        "push NULL\n"
        "push dummy\n"
        "push 1\n"
        "push esi\n"
        "push ebx\n"
        "call _ReadFile@20\n"
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
