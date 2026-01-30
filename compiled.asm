push 1000
pop  R0X 
push 5000
pop  RCX 
call main:
hlt

print:
    push REX
    out
    ret
scan:
    in
    pop RAX
    ret
discriminant:
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
sub
pop RAX
pop RCX
pop RBX
ret
hlt
main:
push RBX
push RCX
push RCX
pop RBX
push RBX
push 0
sub
pop RCX
push 4
pop RGX
push 7
pop RFX
push 4
pop REX
call discriminant:
push RAX
pop RAX
pop RCX
pop RBX
ret
hlt
