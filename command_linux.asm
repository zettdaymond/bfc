;inc ptr
    add esi, 4
/;dec ptr
    sub esi, 4
/;inc value
    add dword [esi], 1
/;dec value
    sub dword [esi], 1
/;putchar
    mov eax, 4
    mov ebx, 1
    mov ecx, esi
    mov edx, 1
    int 0x80
/;getchar
    mov eax, 3
    mov ebx, 1
    mov ecx, esi
    mov edx, 1
    int 0x80
/;start loop
while_NUM: 
    cmp dword [esi], 0
    je near end_while_NUM
/;stop loop
    jmp near while_NUM
end_while_NUM:
