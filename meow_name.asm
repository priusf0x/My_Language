call main
hlt

main:
push RFX
pop [RBX - 1]
push REX
pop [RBX - 0]
push RBX
push RCX
push RCX
pop RBX
push RBX
push 3
sub
pop RCX
pop RCX
pop RBX
ret
