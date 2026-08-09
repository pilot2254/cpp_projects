.code


;


asm_add PROC
    ; assuming that our func call is `asm_add(5, 10);`

    ; when this function is called rcx already holds the 1st argument (5)
    ; and rdx already holds the 2nd argument (10)
    ; just a fixed rule the cpu/compiler follows

    mov rax, rcx    ; overwrite rax value with rcx
    add rax, rdx    ; rax = rax + rdx
    ret             ; rax is automatically the return value
asm_add ENDP


;


asm_sub PROC

    mov rax, rcx
    sub rax, rdx
    ret

asm_sub ENDP


;


asm_max PROC

    cmp rcx, rdx
    jg greater

    mov rax, rdx
    ret

greater:
    mov rax, rcx
    ret

asm_max ENDP


;


asm_sum PROC

    mov rax, 0

loop_start:
    cmp rcx, 0
    jz finish
    add rax, rcx
    dec rcx
    jmp loop_start

finish:
    ret

asm_sum ENDP


;


asm_factorial PROC

    mov rax, 1

loop_start:
    cmp rcx, 0
    jz finish

    imul rax, rcx
    dec rcx

    jmp loop_start


finish:
    ret
    
asm_factorial ENDP


;


asm_array_sum PROC

    mov rax, 0
    mov rsi, 0

loop_start:
    cmp rsi, rdx
    jz finish

    add rax, [rcx + rsi*8]
    inc rsi

    jmp loop_start

finish:
    ret


asm_array_sum ENDP


;


asm_array_double PROC

    mov rsi, 0

loop_start:
    cmp rsi, rdx
    jz  finish

    mov rax, [rcx + rsi*8]   ; load arr[i]
    add rax, rax             ; double it
    mov [rcx + rsi*8], rax   ; store back into arr[i]
    inc rsi

    jmp loop_start

finish:
    ret
asm_array_double ENDP


;


asm_array_max PROC

    mov rax, [rcx]
    mov rsi, 0

loop_start:
    cmp rsi, rdx
    jz finish

    cmp [rcx + rsi*8], rax
    jg greater
    inc rsi
    jmp loop_start

greater:
    mov rax, [rcx + rsi*8]
    inc rsi
    jmp loop_start

finish:
    ret

asm_array_max ENDP


;


asm_array_min PROC

    mov rax, [rcx]
    mov rsi, 0

loop_start:
    cmp rsi, rdx
    jz finish

    cmp [rcx + rsi*8], rax
    jl less
    inc rsi
    jmp loop_start

less:
    mov rax, [rcx + rsi*8]
    inc rsi
    jmp loop_start

finish:
    ret

asm_array_min ENDP


;


asm_array_reverse PROC

    mov rsi, 0  ; front index
    mov r9, rdx
    dec r9      ; back index & len - 1

loop_start:
    cmp rsi, r9
    jge finish          ; stop when front meets or passes back

    mov r8, [rcx + rsi*8]      ; temp = arr[front]
    mov rax, [rcx + r9*8]      ; rax = arr[back]
    mov [rcx + rsi*8], rax     ; arr[front] = arr[back]
    mov [rcx + r9*8], r8       ; arr[back] = temp

    inc rsi
    dec r9

    jmp loop_start

finish:
    ret

asm_array_reverse ENDP

END

; came back home from a long fucking vacation and i forgot everything. im sorry assembly, i still love you
