global main
section .data
	meow dq 232
section .text

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
	sub rsp, 56

;compile args
	mov qword [rbp - 8], rdi
	push rbx

;body
	mov qword [rbp - 56], 1000000000
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

;while condition
.L1:
	mov rbx, qword [rbp - 56]
	push rbx
	mov rbx, 0
	mov rax, rbx
	pop rbx
	cmp rbx, rax
	mov rbx, 0
	mov rax, 1
	cmovg rbx, rax
	cmp rbx, 0
	jz .L0

;while body
	mov rbx, qword [rbp - 56]
	push rbx
	mov rbx, 1
	mov rax, rbx
	pop rbx
	sub rbx, rax
	mov qword [rbp - 56], rbx
	jmp .L1
.L0:
	mov rbx, 0

;epilogue
	pop rbx
	mov rsp, rbp
	pop rbp
	mov rax, rbx
	ret
