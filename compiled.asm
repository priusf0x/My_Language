push 1000
pop  R0X 
push 5000
pop  RCX 
call main:
hlt

determinant:
push RBX
push RCX
push RCX
pop RBX
push RBX
push 3
sub
pop RCX
push RGX
pop [RBX - 2]
push RFX
pop [RBX - 1]
push REX
pop [RBX - 0]
push [RBX - 1]
push [RBX - 1]
mul
push 4
push [RBX - 0]
mul
push [RBX - 2]
mul
push 1
mul
sub
push 0
add
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
push 0
sub
pop RCX
push 32
pop RGX
push 3
pop RFX
push 12
pop REX
call determinant:
push RAX
pop RAX
pop RCX
pop RBX
ret
