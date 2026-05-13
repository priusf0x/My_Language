_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

zzzz:

;prologue
	push rbp
	mov rbp, rsp
	sub rsp, 48

;compile args
	mov qword [rbp - 48], r9
	mov qword [rbp - 40], r8
	mov qword [rbp - 32], rcx
	mov qword [rbp - 24], rdx
	mov qword [rbp - 16], rsi
	mov qword [rbp - 8], rdi
	push rbx

;body
	mov rbx, qword [rbp - 8]

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	mov rax, rbx
	ret

main:

;prologue
	push rbp
	mov rbp, rsp
	sub rsp, 64

;compile args
	mov qword [rbp - 8], rdi
	push rbx

;body
	mov rbx, 321
	mov qword [rbp - 56], rbx
	mov rbx, 1000000000
	push rbx
	mov rbx, qword [rbp - 56]
	mov rax, rbx
	pop rbx
	add rbx, rax
	mov qword [rbp - 64], rbx
	mov rbx, 1
	mov rdi, rbx
	mov rbx, 2
	mov rsi, rbx
	mov rbx, 3
	mov rdx, rbx
	mov rbx, 4
	mov rcx, rbx
	mov rbx, 5
	mov r8, rbx
	mov rbx, 6
	mov r9, rbx
	call zzzz
	mov rbx, rax

;if condition
	mov rbx, qword [rbp - 64]
	push rbx
	mov rbx, 0
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	setg bl
	mov rbx, qword [rbp - 64]
	push rbx
	mov rbx, 0
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	setg bl
	test rbx, rbx
	jz .L0

;if body
	mov rbx, 1

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	mov rax, rbx
	ret
	.L0:
	mov rbx, 0

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	mov rax, rbx
	ret
