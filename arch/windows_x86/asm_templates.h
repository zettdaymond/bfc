#ifndef ASM_TEMPLATES_H
#define ASM_TEMPLATES_H

#include <string>
#include <unordered_map>

std::string templateBegin =
        "; This is an automatically generated NASM assembler\n"
        "; listing by Brainfuck Compiller"

        "STD_OUTPUT_HANDLE equ -11"
        "%define NULL    dword 0"

        "extern  _GetStdHandle@4"
        "extern  _WriteFile@20"
        "extern  _ReadFile@20"
        "extern  _ExitProcess@4"

        "BITS 32"
        "GLOBAL _start"

        "SECTION .text"
        "_start:"
        "    mov esi, dataptr"
        "; hStdOut = GetstdHandle(STD_OUTPUT_HANDLE)"
        "    push STD_OUTPUT_HANDLE"
        "    call _GetStdHandle@4"
        "    mov ebx, eax";

std::string templateEnd =
        "; ExitProcess(0)"
        "    push    0"
        "    call    _ExitProcess@4"
        "SECTION .bss"
        "    dummy: resd 1"
        "    dataptr: resd 30000";

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
        "call _WriteFile@20"
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
