sources = $(wildcard *.c)
objects = $(addsuffix .o, $(basename $(sources)))
target = pseudo-shell

$(target) : $(objects)
	gcc -o $(target) $(objects)

%.o : %.c
	gcc -g -c $< -o $@

clean :
	rm $(target) $(objects)
