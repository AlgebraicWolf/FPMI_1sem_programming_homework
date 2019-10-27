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

push 5
pop ax

push [ax]
inc ax
out
push [ax]
inc ax
out
push [ax]
inc ax
out