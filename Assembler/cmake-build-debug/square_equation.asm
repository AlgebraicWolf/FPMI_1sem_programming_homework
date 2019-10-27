in
in
in
pop cx
pop bx
pop ax

push ax
push 0
je linear
pop ax
pop ax

call disc
push dx
push 0
je single_root
jb two_roots
ja zero

print:
push ax
out
end

print_2:
push bx
out
push ax
out
end

disc:
push bx
push bx
mul
push -1
push 4
push ax
push cx
mul
mul
mul
add
pop dx
ret

linear:
push bx
push 0
je const
pop bx
pop bx
push bx
push cx
push -1
mul
div
pop ax
jmp print

const:
push cx
push 0
je inf
jne zero

inf:
push -1
pop ax
jmp print

zero:
push 44
pop ax
jmp print

single_root:
push 2
push ax
mul
push -1
push bx
mul
div
pop ax
jmp print

two_roots:
push dx
sqrt
pop dx

push 2
push ax
mul

push -1
push bx
mul
push dx
add
div

push 2
push ax
mul

push -1
push bx
mul
push dx
sub
div

pop ax
pop bx
jmp print_2
