push 0
pop ax
push 1
pop [ax+5]

inc ax
push 2
pop [ax+5]

inc ax
push 3
pop [ax+5]

push 0
pop ax

push [ax+5]
inc ax
out
push [ax+5]
inc ax
out
push [ax+5]
inc ax
out