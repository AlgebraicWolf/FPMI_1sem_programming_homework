push 0
pop ax
meow4:
push 0
pop bx
call meow0
push cx
push 900
jbe meow1
ja meow2
meow5:
inc bx
push 32
push bx
jne meow3
inc ax
push ax
push 32
jne meow4
draw
end
meow0:
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
jmp meow5
meow2:
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
push bx
push 63
sub
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
push bx
push 1000
mul
push ax
push 63
sub
add
push 10
mul
push 2
add
pop dx
pix dx
push bx
push 63
sub
push 1000
mul
push ax
push 63
sub
add
push 10
mul
push 2
add
pop dx
pix dx
jmp meow5
