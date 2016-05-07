; This is an automatically generated NASM assembler
; listing by Brainfuck Compiller

STD_OUTPUT_HANDLE equ -11
%define NULL    dword 0

extern  _GetStdHandle@4
extern  _WriteFile@20
extern  _ReadFile@20
extern  _ExitProcess@4

BITS 32
GLOBAL _start

SECTION .text
_start:
    mov esi, dataptr
; hStdOut = GetstdHandle(STD_OUTPUT_HANDLE)
    push STD_OUTPUT_HANDLE
    call _GetStdHandle@4
    mov ebx, eax
{SOURCE}
; ExitProcess(0)
    push    0
    call    _ExitProcess@4
SECTION .bss
    dummy: resd 1
    dataptr: resd 30000