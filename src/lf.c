/*
 * LasForceC.c
 *
 *  Created on: Feb 23, 2015
 *      Author: gijs
 */

#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/lf.h"
#include "../include/lasforce/ilda/ILDA.h"
#include "../include/lasforce/ilda/serialize.h"
#include "../include/lasforce/ilda/deserialize.h"
#include "../include/lasforce/lf_socket.h"

int playing = 0;
ilda_message* toplay;
pthread_mutex_t toplay_lock = PTHREAD_MUTEX_INITIALIZER;

char* concat(char *s1, char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); //+1 for the zero-terminator
	if (result == NULL)
		error("Can't concat s1 and s2\n");
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void error(const char *msg) {
	perror(msg);
	exit(1);
}

void addToPlay(ILDA ilda) {
	if (playing == 0) {
		pthread_mutex_lock(&toplay_lock);
		toplay = malloc(sizeof(&ilda));
		pthread_mutex_unlock(&toplay_lock);
	}
}

void* handleMessages(void* param) {
	printf("Message handler started.\n");
	int listener_d = createSocket();
	struct sockaddr_storage client_addr;
	unsigned int address_size = sizeof(client_addr);
	while(1) {
		printf("Waiting for client connections....\n");
		int connect_d = accept(listener_d, (struct sockaddr*) &client_addr,
				&address_size);
		if (connect_d == -1) {
			error("Can't open secondary socket");
		}
		int index = 0;
		ilda_message message;
		do {
			message = readSocketMessage(connect_d);
			if(message.length > 0) {
				printf("Serializing...%i\n", index++);
				ILDA ilda = serialize(message.content, message.length);
				addToPlay(ilda);
				printf("Content: %s\n", message.content);
//				destroyIlda(ilda);
			}
//			sleep(1);


		} while (message.more);
		close(connect_d);
	}
	return NULL;
}

void* ildaPlayer(void* param) {
	printf("ILDA Player started.\n");
	while(1) {
		sleep(1);
		printf("Checking table\n");
	}
	return NULL;
}

int main(int argc, const char * argv[]) {
	printf("Starting LasForce.\n");

	pthread_t message_handler, ilda_player;
	if (pthread_create(&message_handler,NULL, handleMessages, NULL))
		perror("Can't create message_handler thread");
	if (pthread_create(&ilda_player, NULL, ildaPlayer, NULL))
		perror("Can't create ilda_player thread");
	void* result;
	pthread_join(message_handler, &result);
	pthread_join(ilda_player, &result);
	return 0;
}
