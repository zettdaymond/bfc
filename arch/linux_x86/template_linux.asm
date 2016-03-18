; This is an automatically generated NASM assembler
; listing by Brainfuck Compiller
BITS 32
GLOBAL _start
SECTION .bss
    DATAPTR: resd 30000
SECTION .text
_start:
    mov esi, DATAPTR
{SOURCE}
;exit
    mov eax, 1
    mov ebx , 0x5D
    int 0x80
