print:              
    push rbp         
    mov rbp, rsp    

    cmp rdi, 0
    jg .L0
    neg rdi
    push rdi
    push '-'

    mov rax, 1      
    mov rdi, 1      
    mov rsi, rsp    
    mov rdx, 1      
    syscall
    add rsp, 8
    pop rdi
.L0: 

    mov rcx, 10     
    mov rax, rdi    
    
.L1:
    xor rdx, rdx    
    div rcx         
                        
    add rdx, '0'    
    push rdx         
    cmp rax, 0       
    jne .L1          

.L2:
    mov rax, 1      
    mov rdi, 1      
    mov rsi, rsp    
    mov rdx, 1      
    syscall
    
    add rsp, 8      
    cmp rbp, rsp    
    jne .L2 

    pop rbp
    ret