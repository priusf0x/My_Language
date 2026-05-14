global _start
_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

factorial:

;prologue
	push rbp
	mov rbp, rsp
	sub rsp, 48

;compile args
	mov qword [rbp - 8], rdi
	push rbx

;body

;if condition
	mov rbx, qword [rbp - 8]
	push rbx
	mov rbx, 0
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	sete bl
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
	.L0:
	mov rbx, qword [rbp - 8]
	push rbx
	mov rbx, qword [rbp - 8]
	push rbx
	mov rbx, 1
	mov rax, rbx
	pop rbx
	sub rbx, rax
	mov rdi, rbx
	call factorial
	mov rbx, rax
	mov rax, rbx
	pop rbx
	imul rbx, rax
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
	mov qword [rbp - 8], rdi
	push rbx

;body
	mov rbx, 6
	mov rdi, rbx
	call factorial
	mov rbx, rax
	mov rax, rbx

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	ret
