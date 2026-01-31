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

set:
    push R0X
    push 30
    push RFX
    mul
    push REX
    add
    pop R0X
    push RGX
    pop [R0X]
    pop R0X
    ret

setscreen:
    push R0X
    push 0
    pop R0X
__SETSCREEN_1:
    push 32 
    pop [R0X]
    push R0X
    push 1
    add
    pop R0X
    push R0X
    push 900
    jne __SETSCREEN_1:
    pop R0X
    ret

draw:
    push R0X
    push 30
    pop R0X
__DRAW_1:
    push 10
    pop [R0X]
    push R0X
    push 30
    add
    pop R0X
    push 900
    push R0X
    jne __DRAW_1:
    draw
    pop R0X
    ret

putc:
    push R0X
    push 901
    pop R0X
    push [R0X]
    pop R0X
    push REX
    pop [R0X]
    push R0X
    push 1
    add
    push 901
    pop R0X
    pop [R0X]
    pop R0X
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
push 72
pop REX
call putc:
push RAX
pop RDX
push 101
pop REX
call putc:
push RAX
pop RDX
push 108
pop REX
call putc:
push RAX
pop RDX
push 108
pop REX
call putc:
push RAX
pop RDX
push 111
pop REX
call putc:
push RAX
pop RDX
push 44
pop REX
call putc:
push RAX
pop RDX
push 32
pop REX
call putc:
push RAX
pop RDX
push 119
pop REX
call putc:
push RAX
pop RDX
push 111
pop REX
call putc:
push RAX
pop RDX
push 114
pop REX
call putc:
push RAX
pop RDX
push 108
pop REX
call putc:
push RAX
pop RDX
push 100
pop REX
call putc:
push RAX
pop RDX
push 33
pop REX
call putc:
push RAX
pop RDX
call draw:
push RAX
pop RDX
push 0
pop RAX
pop RCX
pop RBX
ret
hlt
