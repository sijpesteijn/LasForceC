/*
 * LasForceC.c
 *
 *  Created on: Feb 23, 2015
 *      Author: gijs
 */

#include <arpa/inet.h>
#include <unistd.h>
#include "../include/lf.h"
#include "../include/lasforce/ilda/ILDA.h"
#include "../include/lasforce/ilda/serialize.h"
#include "../include/lasforce/ilda/deserialize.h"
#include "../include/lasforce/lf_socket.h"


char* concat(char *s1, char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); //+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void error(const char *msg) {
	perror(msg);
	exit(1);
}

void handleSocketsRequestMessages(int listener_d) {
	struct sockaddr_storage client_addr;
	unsigned int address_size = sizeof(client_addr);

	while (1) {
		int connect_d = accept(listener_d, (struct sockaddr*) &client_addr,
				&address_size);
		if (connect_d == -1) {
			error("Can't open secondary socket");
		}
		if (!fork()) {
			close(listener_d);
			int index = 0;
			ilda_message smsg;
			do {
				smsg = readSocketMessage(connect_d);
				if (smsg.length > 0) {
	//				printf("Content: %s\n", smsg.content);
					printf("Length: %lu\n", smsg.length);
					printf("Meer: %i\n", smsg.more);
					printf("Index: %i\n", index++);
					printf("Serializing...\n");
					ILDA ilda = serialize(smsg.content, smsg.length);

					printf("Deserializing...\n");
					char* deserialized = deserialize(ilda);
					printf("ILDA: %s\n", deserialized);

					destroyIlda(ilda);
				}

			} while (smsg.more);
			printf("Client socket closed\n\n\n");
			close(connect_d);
			exit(0);
		}
		close(connect_d);
	}
}

int main(int argc, const char * argv[]) {
	printf("Starting LasForce.\n");

	int listener_d = createSocket();
	printf("Waiting for client connections....\n");
	handleSocketsRequestMessages(listener_d);

	return 0;
}
