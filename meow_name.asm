call main
hlt

push 23423
push 133
mul
push 133
push [0]
mul
je .L0:
push 0
jmp .L1:
.L0:
push 1
.L1:
pop RDX
