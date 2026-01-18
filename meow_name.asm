call main
hlt

push 123
pop [0]
push [0]
pop RDX
push [0]
push 31
ja .L1:
push 0
jmp .L2:
.L1:
push 1
.L2:
push 0
je .L0:
push [0]
push 32
sub
pop [0]
push [0]
pop RDX
.L0:
