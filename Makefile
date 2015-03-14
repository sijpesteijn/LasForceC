LasForce: 
	gcc -I./include/ -ljansson -lpthread -o lasforce-c ./src/lf.c ./src/lasforce/lf_socket.c
	
clean:
	$(RM) lasforce-c