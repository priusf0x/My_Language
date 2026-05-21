scan:
    push rbp            
    mov rbp, rsp        
    sub rsp, 32         

    mov rax, 0
    mov rdi, 0          
    mov rsi, rsp        
    mov rdx, 32        
    syscall             

    mov rcx, rax        
    mov rsi, rsp        
    xor rax, rax        
    mov rbx, 10        

.loop:
    movzx rdx, byte [rsi] 
    cmp dl, 10          
    je .done

    sub dl, '0'         
    imul rax, rbx       
    add rax, rdx        

.next:
    inc rsi             
    loop .loop          

.done:
    mov rsp, rbp
    pop rbp             
    ret                 