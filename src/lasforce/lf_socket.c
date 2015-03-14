/*
 * lf_socket.c
 *
 *  Created on: Feb 24, 2015
 *      Author: gijs
 */

#include "../../include/lasforce/lf_socket.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int createSocket() {
	int reuse = 1;
	int listener_d = socket(PF_INET, SOCK_STREAM, 0);
	if (listener_d == -1)
		error("Can't open socket");
	if (setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse,
			sizeof(int)) == -1) {
		error("Can't set the reuse option on the socket");
	}
	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = (in_port_t) htons(5555);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	int connection = bind(listener_d, (struct sockaddr*) &name, sizeof(name));
	if (connection == -1) {
		error("Can't bind to socket");
	}

	if (listen(listener_d, 10) == -1) {
		error("Can't listen");
	}

	return listener_d;
}

int getMessageLength(int connect_d) {
	int message_size_length = 13;
	char message_size[message_size_length];
	int n = read(connect_d, message_size, message_size_length - 1);
	if (n < 0)
		error("Can't reading size from socket");
	message_size[12] = '\0';
	return atol(message_size);
}

ilda_message readSocketMessage(int connect_d) {
	unsigned long message_length = getMessageLength(connect_d);
	if (message_length == 0) {
		ilda_message smsg = { "", 0, 0 };
		return smsg;
	}
	char* message = malloc(message_length + 1);
	if (message == NULL) {
		printf("Can't allocate memory for message");
		exit(1);
	}
	bzero(message, message_length + 1);

	int socket_size = 4096; //32768;
	if (socket_size > message_length) {
		socket_size = message_length;
	}

	unsigned long bytes_read = 0;
	int more = 1, n = 0;
	while (more && bytes_read < message_length) {
		char buffer[socket_size];
		n = read(connect_d, buffer, socket_size);
		if (n < 0)
			error("ERROR reading message from socket");
		if (n == 0) {
			more = 0;
		}
		buffer[n] = '\0';
		if (bytes_read == 0) {
			strcpy(message, buffer);
		} else {
			strcat(message, buffer);
		}
		bytes_read += n;
	}
	ilda_message smsg = { message, message_length, more };
//	printf("Sending OK\n");
	n = write(connect_d, "OK\n", 3);
	if (n < 0)
		error("ERROR sending ackownlegde message");
	return smsg;
}

