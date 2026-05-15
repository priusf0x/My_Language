putc:
    push rdi
    mov rax, 1      
    mov rdi, 1      
    mov rsi, rsp    
    mov rdx, 1      
    syscall
    add rsp, 8
    ret