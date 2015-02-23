/*
 * LasForce.h
 *
 *  Created on: Feb 23, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_H_
#define INCLUDE_LASFORCE_H_

#include <stdlib.h>
#include <string.h>

void error(const char *msg) {
	perror(msg);
	exit(1);
}

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int readClient(int socket, char *buf, int len) {
	char *s = buf;
	int slen = len;
	int c = recv(socket, s, slen, 0);
	while ((c > 0) && (s[c-1] != '\n')) {
		s += c; slen -= c;
		c = recv(socket, s, slen, 0);
	}
	if (c < 0)
		return c;
	else if (c == 0)
		buf[0] = '\0';
	else
		s[c-1]='\0';
	return len - slen;
}

int createSocket() {
	int reuse = 1;
	int listener_d = socket(PF_INET, SOCK_STREAM,0);
	if (listener_d == -1)
		error("Can't open socket");
	if (setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) {
		error("Can't set the reuse option on the socket");
	}
	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = (in_port_t)htons(5555);
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


char* readSocketMessage(int connect_d) {
	int message_size_length = 13;
	char message_size[message_size_length];
	int n = read(connect_d, message_size, message_size_length-1);
	if (n < 0)
		error("Can't reading size from socket");
	message_size[12] = '\0';
	printf("Message size string: %s\n", message_size);
	unsigned long message_length = atol(message_size);
	if (message_length == 0) {
		// Socket closed from the client side
		printf("Client closed connection.\n");
		return 0;
	}
	printf("Message size: %lu\n", message_length);
//	char message[message_length+1];
	char *message;
	int socket_size = 2048; //32768;
	if (socket_size > message_length) {
		socket_size = message_length;
	}
	unsigned long bytes_read = 0;
	int klaar = 1;
	while(klaar && bytes_read < message_length) {
		char buffer[socket_size];
		n = read(connect_d, buffer, socket_size);
		if (n < 0)
			error("ERROR reading message from socket");
		if (n == 0) {
			printf("kl");
			klaar = 0;
		}
		bytes_read += n;
		buffer[n] = '\0';
		printf("Read %i\n", n);
		printf("Bytes read: %lu\n", bytes_read);
		printf("Buf: %s\n", buffer);
//		concat(message, buffer);
	}
	printf("Klaar\n");
//	printf("Message: %.500s\n",message);
	return "";
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
			readSocketMessage(connect_d);
			printf("Client socket closed");
			close(connect_d);
			exit(0);
		}
		close(connect_d);
	}
}

#endif /* INCLUDE_LASFORCE_H_ */
