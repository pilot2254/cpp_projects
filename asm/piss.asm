.data
    hello_msg db "hello world", 0   ; db = "define byte", 0 - null terminator
                                    ; string as raw bytes + null terminator

.code

asm_hello_world PROC

    lea rax, hello_msg ; lea = load effective address, puts it into rax
    ret

    ; lea is like mov but loads an address instead of a value at that address,
    ; similar to &hello_msg in c++ rather than hello_msg itself

asm_hello_world ENDP


;


; additional notes:
; https://gist.github.com/dumpmycode/f541e9c43c7ab25e360a01bdde1f8548

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


;


asm_array_contains PROC

    mov rax, 0
    mov rsi, 0

loop_start:
    cmp rsi, rdx
    jz finish

    cmp r8, [rcx + rsi * 8]
    je found
    
    inc rsi
    jmp loop_start

found:
    mov rax, 1
    jmp finish

finish:
    ret

asm_array_contains ENDP


;


asm_array_count_even PROC

    mov rax, 0
    mov rsi, 0

loop_start:
    cmp rsi, rdx
    jz finish

    mov r9, [rcx + 8 * rsi]

    test r9, 1
    jz is_even

    inc rsi
    jmp loop_start

is_even:
    inc rax
    inc rsi
    jmp loop_start

finish:
    ret

asm_array_count_even ENDP


;


asm_array_bubble_sort PROC

; rcx arr
; rdx len
; rsi outer loop
; r8 innter loop
; r9 temp

; bubble sort in C++:

; void sort(int array[], int size){
;         int temp;
;         for(int i = 0; i < size - 1; i++)
;         {
;                 for(int j = 0; j < size - i - 1; j++)
;                 {
;                         if(array[j] > array[j + 1])
;                         { 
;                                 temp = array [j];
;                                 array[j] = array[j + 1];
;                                 array[j + 1] = temp;
;                         }
;                 }
;         }
; }

    mov rax, 0
    mov r8, 0
    mov rsi, 0

outer_loop:
    cmp rsi, rdx
    je finish

    mov r10, rdx
    dec r10          ; r10 = len - 1, the real inner-loop limit
    jmp inner_loop

inner_loop:    
    mov r9, [rcx + 8 * r8]
    cmp r9, [rcx + 8 * r8 + 8]
    jg swap
    add r8, 1
    cmp r8, r10
    je outer_pass_done
    jmp inner_loop

outer_pass_done:
    inc rsi
    mov r8, 0
    jmp outer_loop

swap:
    mov r9, [rcx + 8 * r8]
    mov rax, [rcx + 8 * r8 + 8]
    mov [rcx + 8 * r8], rax
    mov [rcx + 8 * r8 + 8], r9

    add r8, 1
    jmp inner_loop

finish:
    ret

asm_array_bubble_sort ENDP


;


asm_strlen PROC
    ; rcx str
    mov rsi, 0

loop_start:
    movzx rax, byte ptr [rcx + rsi] ; loads a single byte from memory into rax
                                    ; zero-extending the rest (register is 8 bytes and char is 1 byte)
    cmp rax, 0
    jz  finish

    inc rsi
    jmp loop_start

finish:
    mov rax, rsi
    ret

asm_strlen ENDP


;


asm_string_char_count PROC

    mov rsi, 0
    mov rax, 0

loop_start:
    movzx r8, byte ptr [rcx + rsi]

    cmp r8, 0
    je finish

    cmp r8, rdx
    je found

    inc rsi
    jmp loop_start

found:
    inc rax
    inc rsi
    jmp loop_start

finish:
    ret

asm_string_char_count ENDP


;


asm_string_reverse PROC

    ; rax res
    ; rcx str
    ; rdx len
    ; rsi str index
    ; r8 temp

    mov rax, rcx

find_length:
    movzx rax, byte ptr [rcx + rsi]
    cmp rax, 0
    jz  pre_loop

    inc rsi
    jmp find_length

pre_loop:
    xor rsi, rsi
    jmp loop_start

loop_start:

swap:
    movzx r8, byte ptr [rcx + rsi]
    movzx 

finish:
    mov rax, rcx
    ret

asm_string_reverse ENDP


;


asm_fibonacci PROC

    cmp rcx, 0
    jne check_one
    mov rax, 0
    ret

check_one:
    cmp rcx, 1
    jne compute
    mov rax, 1
    ret

compute:
    mov rax, 0
    mov rdx, 1
    mov r8, 1

loop_start:
    cmp r8, rcx
    je  finish

    mov r9, rax
    mov rax, rdx
    add rdx, r9

    inc r8
    jmp loop_start

finish:
    ret

asm_fibonacci ENDP

END

; came back home from a long fucking vacation and i forgot everything. im sorry assembly, i still love you
