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

herons_square:
push RBX
push RCX
push RCX
pop RBX
push RBX
push 3
sub
pop RCX
push REX
pop [RBX - 0]
push 10
pop [RBX - 1]
push [RBX - 0]
pop [RBX - 2]
.L1:
push [RBX - 1]
push 0
ja .L3:
push 0
jmp .L4:
.L3:
push 1
.L4:
push 0
je .L2:
push [RBX - 1]
push 1
sub
pop [RBX - 1]
push [RBX - 1]
pop RDX
push [RBX - 2]
push [RBX - 0]
push [RBX - 2]
div
add
push 2
div
pop [RBX - 2]
push [RBX - 2]
pop RDX
jmp .L1:
.L2:
.L0:
push [RBX - 2]
pop RAX
pop RCX
pop RBX
ret
hlt
solve_linear:
push RBX
push RCX
push RCX
pop RBX
push RBX
push 2
sub
pop RCX
push RFX
pop [RBX - 1]
push REX
pop [RBX - 0]
push [RBX - 1]
push [RBX - 0]
div
pop RAX
pop RCX
pop RBX
ret
hlt
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
push 4
sub
pop RCX
call scan:
push RAX
pop [RBX - 0]
call scan:
push RAX
pop [RBX - 1]
call scan:
push RAX
pop [RBX - 2]
push [RBX - 0]
push 0
je .L7:
push 0
jmp .L8:
.L7:
push 1
.L8:
push 0
je .L6:
push [RBX - 1]
push 0
je .L11:
push 0
jmp .L12:
.L11:
push 1
.L12:
push 0
je .L10:
push [RBX - 2]
push 0
je .L15:
push 0
jmp .L16:
.L15:
push 1
.L16:
push 0
je .L14:
push 0
pop REX
call print:
push RAX
pop RDX
push 0
pop REX
call print:
push RAX
pop RDX
push 0
pop REX
call print:
push RAX
pop RDX
jmp .L13:
.L14: 
push -1
pop REX
call print:
push RAX
pop RDX
push -1
pop REX
call print:
push RAX
pop RDX
push -1
pop REX
call print:
push RAX
pop RDX
.L13:
jmp .L9:
.L10:
push [RBX - 2]
pop RFX
push [RBX - 1]
pop REX
call solve_linear:
push RAX
pop REX
call print:
push RAX
pop RDX
.L9:
jmp .L5:
.L6:
push [RBX - 2]
pop RGX
push [RBX - 1]
pop RFX
push [RBX - 0]
pop REX
call discriminant:
push RAX
pop [RBX - 3]
push [RBX - 3]
push 0
je .L19:
push 0
jmp .L20:
.L19:
push 1
.L20:
push 0
je .L18:
push 0
pop REX
call print:
push RAX
pop RDX
jmp .L17:
.L18:
push [RBX - 3]
push 0
jb .L23:
push 0
jmp .L24:
.L23:
push 1
.L24:
push 0
je .L22:
push -1
pop REX
call print:
push RAX
pop RDX
push -1
pop REX
call print:
push RAX
pop RDX
jmp .L21:
.L22:
push [RBX - 1]
push [RBX - 3]
pop REX
call herons_square:
push RAX
add
push 2
div
push [RBX - 0]
div
pop REX
call print:
push RAX
pop RDX
push [RBX - 1]
push [RBX - 3]
pop REX
call herons_square:
push RAX
sub
push 2
div
push [RBX - 0]
div
pop REX
call print:
push RAX
pop RDX
.L21:
.L17:
.L5:
hlt
