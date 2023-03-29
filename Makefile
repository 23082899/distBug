# Makefile Compile all .c into .x
ALL:  $(patsubst %.c,%.x, $(wildcard *.c))
	
%.x: %.c
	gccsim -o $*.x $*.c
	
clean:
	$(RM) *.x
