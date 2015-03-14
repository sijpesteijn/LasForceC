LasForce: 
	gcc -I./include/ -ljansson -o LasForce ./src/lf.c ./src/lasforce/lf_socket.c
	
clean:
	$(RM) LasForce