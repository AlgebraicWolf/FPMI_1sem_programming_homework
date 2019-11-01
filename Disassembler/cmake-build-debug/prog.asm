push 0
pop ax
meow0:
push 0
pop bx
call meow0
push cx
push 900
jbe meow0
ja meow0
meow0:
inc bx
push 32
push bx
jne meow0
inc ax
push ax
push 32
jne meow0
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
jmp meow0
meow0:
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
jmp meow0
