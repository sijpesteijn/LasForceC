LasForce: 
	gcc -I./include/ -ljansson -lpthread -o lasforce-c ./src/lasforce/ilda/ILDA.c ./src/lasforce/serialize/serialize.c ./src/lasforce/serialize/serializeCommands.c ./src/lasforce/serialize/serializeILDA.c ./src/lf.c ./src/lasforce/lf_socket.c
	
clean:
	$(RM) lasforce-c