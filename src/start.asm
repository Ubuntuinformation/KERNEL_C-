[bits 32]
; Multiboot header (NASM Intel syntax)
section .multiboot
    align 4
    dd 0x1BADB002          ; magic
    dd 0x00010003          ; flags (align modules on page, memory info)
    dd -(0x1BADB002 + 0x00010003)

section .text
global start
extern kernel_main

start:
    ; set up a simple stack and call kernel_main
    mov esp, 0x90000
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
