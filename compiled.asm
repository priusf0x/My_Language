global _start
_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

fibonacci:

;prologue
	push rbp
	mov rbp, rsp
	sub rsp, 48

;compile args
	mov qword [rbp + -8], rdi
	push rbx

;body

;if condition
	mov rbx, [rbp - 8]
	push rbx
	mov rbx, 0
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	sete BL
	test rbx, rbx
	jz .L0

;if body
	mov rbx, 1
	mov rax, rbx

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	ret
	jmp .L1
.L0:

;if condition
	mov rbx, [rbp - 8]
	push rbx
	mov rbx, 1
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	sete BL
	test rbx, rbx
	jz .L2

;if body
	mov rbx, 1
	mov rax, rbx

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	ret
	jmp .L3
.L2:
.L3:
.L1:
	mov rbx, [rbp - 8]
	push rbx
	mov rbx, 1
	mov rax, rbx
	pop rbx
	sub rbx, rax
	mov rdi, rbx
	call fibonacci
	mov rbx, rax
	push rbx
	mov rbx, [rbp - 8]
	push rbx
	mov rbx, 2
	mov rax, rbx
	pop rbx
	sub rbx, rax
	mov rdi, rbx
	call fibonacci
	mov rbx, rax
	mov rax, rbx
	pop rbx
	add rbx, rax
	mov rax, rbx

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

main:

;prologue
	push rbp
	mov rbp, rsp
	sub rsp, 48

;compile args
	push rbx

;body
	call scan
	mov rbx, rax
	mov rdi, rbx
	call fibonacci
	mov rbx, rax
	mov rdi, rbx
	call print
	mov rbx, rax
	mov rbx, 0
	mov rax, rbx

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	ret
