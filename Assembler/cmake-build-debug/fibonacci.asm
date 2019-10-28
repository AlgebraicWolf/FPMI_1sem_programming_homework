in
pop bx

push 1
pop ax

push 1

push ax
push bx

cycle:
pop ax
pop ax

inc ax
push ax
mul

push ax
push bx
jne cycle

pop ax
pop ax

out