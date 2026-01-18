push 1000
pop RCX
call main
hlt

fibs:
push RBX
push RCX
push RCX
pop RBX
push RBX
push 1
sub
pop RCX
push REX
pop [RBX - 0]
push [RBX - 0]
push 2
jb .L1:
push 0
jmp .L2:
.L1:
push 1
.L2:
push 0
je .L0:
push 1
pop RAX
pop RCX
pop RBX
ret
.L0:
push [RBX - 0]
push 1
sub
pop REX
call fibs
push RAX
push [RBX - 0]
push 2
sub
pop REX
call fibs
push RAX
mul
pop RAX
pop RCX
pop RBX
ret
main:
push RBX
push RCX
push RCX
pop RBX
push RBX
push 3
sub
pop RCX
push RFX
pop [RBX - 1]
push REX
pop [RBX - 0]
push 10
pop [RBX - 2]
push [RBX - 2]
pop REX
call fibs
push RAX
pop RAX
pop RCX
pop RBX
ret
