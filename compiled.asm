global _start
_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

main:

;prologue
	push rbp
	mov rbp, rsp
	sub rsp, 88

;compile args
	push rbx

;body
	mov rbx, 30
	mov qword [rbp - 56], rbx
	mov rbx, 30
	mov qword [rbp - 64], rbx
	mov rbx, 7
	mov qword [rbp - 72], rbx
	mov rbx, 0
	mov qword [rbp - 80], rbx
	mov rbx, 0
	mov qword [rbp - 88], rbx

;while condition
.L1:
	mov rbx, qword [rbp - 88]
	push rbx
	mov rbx, qword [rbp - 64]
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	setl bl
	test rbx, rbx
	jz .L0

;while body
	mov rbx, 0
	mov qword [rbp - 80], rbx

;while condition
.L3:
	mov rbx, qword [rbp - 80]
	push rbx
	mov rbx, qword [rbp - 56]
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	setl bl
	test rbx, rbx
	jz .L2

;while body

;if condition
	mov rbx, qword [rbp - 80]
	push rbx
	mov rbx, qword [rbp - 56]
	push rbx
	mov rbx, 2
	mov rax, rbx
	pop rbx
	mov rcx, rax
	mov rax, rbx
	cqo
	idiv rcx
	mov rbx, rax
	mov rax, rbx
	pop rbx
	sub rbx, rax
	push rbx
	mov rbx, qword [rbp - 80]
	push rbx
	mov rbx, qword [rbp - 56]
	push rbx
	mov rbx, 2
	mov rax, rbx
	pop rbx
	mov rcx, rax
	mov rax, rbx
	cqo
	idiv rcx
	mov rbx, rax
	mov rax, rbx
	pop rbx
	sub rbx, rax
	mov rax, rbx
	pop rbx
	imul rbx, rax
	push rbx
	mov rbx, qword [rbp - 88]
	push rbx
	mov rbx, qword [rbp - 64]
	push rbx
	mov rbx, 2
	mov rax, rbx
	pop rbx
	mov rcx, rax
	mov rax, rbx
	cqo
	idiv rcx
	mov rbx, rax
	mov rax, rbx
	pop rbx
	sub rbx, rax
	push rbx
	mov rbx, qword [rbp - 88]
	push rbx
	mov rbx, qword [rbp - 64]
	push rbx
	mov rbx, 2
	mov rax, rbx
	pop rbx
	mov rcx, rax
	mov rax, rbx
	cqo
	idiv rcx
	mov rbx, rax
	mov rax, rbx
	pop rbx
	sub rbx, rax
	mov rax, rbx
	pop rbx
	imul rbx, rax
	push rbx
	mov rbx, 4
	mov rax, rbx
	pop rbx
	imul rbx, rax
	mov rax, rbx
	pop rbx
	add rbx, rax
	push rbx
	mov rbx, qword [rbp - 72]
	push rbx
	mov rbx, qword [rbp - 72]
	mov rax, rbx
	pop rbx
	imul rbx, rax
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	setl bl
	test rbx, rbx
	jz .L4

;if body
	mov rbx, 35
	mov rdi, rbx
	call putc
	mov rbx, rax
	jmp .L5
.L4:
	mov rbx, 32
	mov rdi, rbx
	call putc
	mov rbx, rax
.L5:
	mov rbx, qword [rbp - 80]
	push rbx
	mov rbx, 1
	mov rax, rbx
	pop rbx
	add rbx, rax
	mov qword [rbp - 80], rbx
	jmp .L3
.L2:
	mov rbx, qword [rbp - 88]
	push rbx
	mov rbx, 1
	mov rax, rbx
	pop rbx
	add rbx, rax
	mov qword [rbp - 88], rbx
	mov rbx, 10
	mov rdi, rbx
	call putc
	mov rbx, rax
	jmp .L1
.L0:
	mov rbx, 0
	mov rax, rbx

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	ret
