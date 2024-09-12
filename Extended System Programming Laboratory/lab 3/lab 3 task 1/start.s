section .data
    newline db 10
    Infile dd 0  ; default stdin = 0
    Outfile dd 1 ; default stdout = 1

section .bss
    input resb 1

section .text
    global main
    extern strlen

prepInput:
    pushad
    mov eax, 5     ; sys_open              
    mov ebx, ecx   ; -I{file}        
    inc ebx
    inc ebx        
    xor ecx, ecx   ; ecx = 0 ===> READ_ONLY                 
    int 0x80                        
    mov dword [Infile], eax         
    popad
    jmp check

prepOutput:
    pushad
    mov eax, 5    ; sys_open                 
    mov ebx, ecx  ; -O{file}
    ;mov ebx, 2    ; {file} (alternative)
    inc ebx
    inc ebx
    mov ecx, 1    ; O_WRONLY
    or ecx, 64    ; (O_WRONLY | O_CREAT)
    mov edx, 777o ; Permissions (write)
    int 0x80  
    mov dword [Outfile], eax        
    popad
    jmp check

main:
    mov edi, dword [esp + 8]  ; argv
    mov esi, dword [esp + 4]  ; argc
    xor edx, edx              ; counter = 0

printing:
    loop:
    mov ecx, dword [edi + edx * 4]  ; what to print

    ; Print argv[i], get length then print
    prnt:
    pushad
    mov ebx, 1               ; filed = stdout = 1
    mov ecx, dword [edi + edx * 4]  ; load string to print
    push ecx
    call strlen              ; len of string
    pop ecx
    mov edx, eax             ; len
    mov eax, 4               ; write syscall
    int 0x80

    ; Print newline
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80
    popad

    ; Check for input and output flags
    inp:
        cmp word [ecx], "-I"            
        je prepInput   
    out:              
        cmp word [ecx], "-O"          
        je prepOutput

check:
    inc edx    ; counter += 1
    cmp edx, esi
    jne loop

encoder:
    ReadChar:
        mov eax, 3                ; sys_read
        mov ebx, dword [Infile]   ; stdin
        mov ecx, input            ; where to store char
        mov edx, 1                ; number of bytes to read
        int 0x80                  ; syscall

    ; Check for end of file
    eof:
        sub eax, 0
        jle exit

    ; CheckRange:
    ; Check if character is between 'A' - 'z'
        movzx eax, byte [input]
        cmp eax, 'A'
        jl print_character     ; If below 'A', print character as is
        cmp eax, 'z'
        jg print_character     ; If above 'z', print character as is

        inc byte [input]       ; Increment character by 1

    print_character:
        mov eax, 4
        mov ebx, dword [Outfile] 
        mov ecx, input
        mov edx, 1
        int 0x80

        jmp encoder

    ; If character is not in range 'A' - 'z', print character as is
    ; not_in_range:
    ;     mov eax, 4
    ;     mov ebx, dword [Outfile] 
    ;     mov ecx, input
    ;     mov edx, 1
    ;     int 0x80
    ;     jmp encoder

    exit:
        mov eax, 1   ; exit syscall              
        mov ebx, 0   ; successful exit
        int 0x80
