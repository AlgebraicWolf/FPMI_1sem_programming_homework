push 0
pop ax
outer_cycle:
push 0
pop bx

inner_cycle:
inc bx

call dist

push cx
push 900

jbe black
ja white

cont:
push 63
push bx
jne inner_cycle

inc ax
push ax
push 63
jne outer_cycle

draw
end

dist:
push 32
push ax
sub
pop cx
push cx
push cx
mul
push 32
push bx
sub
pop cx
push cx
push cx
mul
add
pop cx
ret

black:
push bx
push 1000
mul
push ax
add
push 10
mul
pop dx
pix dx
jmp cont

white:
push bx
push 1000
mul
push ax
add
push 10
mul
push 2
add
pop dx
pix dx
jmp cont