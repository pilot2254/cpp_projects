.code


;


asm_add PROC
    ; assuming that our func call is `asm_add(5, 10);`

    ; when this function is called rcx already holds the 1st argument (5)
    ; and rdx already holds the 2nd argument (10)
    ; just a fixed rule the cpu/compiler follows (not something we write)   

    mov rax, rcx    ; copy the 1st argument (5) into rax
                    ; we need it in rax because thats the register well do the addition in

    add rax, rdx    ; rax = rax + rdx, so rax = 15
                    ; rax now holds the result

    ret             ; return to main()
                    ; whatever value is in rax right now becomes the function's return value
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


END

; came back from a long fucking vacation and i forgot everything. im sorry assembly, i still love you
