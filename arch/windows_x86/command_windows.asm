;inc ptr
    add esi, 4
/;dec ptr
    sub esi, 4
/;inc value
    add dword [esi], 1
/;dec value
    sub dword [esi], 1
/;putchar
    push NULL
    push dummy
    push 1
    push esi
    push ebx
    call _WriteFile@20
/;getchar
    push NULL
    push dummy
    push 1
    push esi
    push ebx
    call _ReadFile@20
/;start loop
w_NUM: 
    cmp dword [esi], 0
    je near ew_NUM
/;stop loop
    jmp near w_NUM
ew_NUM:
