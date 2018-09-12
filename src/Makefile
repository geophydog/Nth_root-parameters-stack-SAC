OBJ = para_stack.o sacio.o

para_stack : $(OBJ)
	cc -o para_stack $(OBJ) -lm

$(OBJ) : sacio.h

clean : 
	rm -f $(OBJ) para_stack
