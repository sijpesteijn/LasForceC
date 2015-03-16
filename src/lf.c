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
#include "../include/lasforce/serialize/serialize.h"
#include "../include/lasforce/ilda/deserialize.h"
#include "../include/lasforce/lf_socket.h"

int playing = 0;
socket_message* toplay;
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

Command* addOkResponse() {
	Command* command = malloc(sizeof(Command));
	command->command = "OK";
	command->command_length = 2;
	return command;
}

ResponseCommands* getResponseCommands(Command* command) {
	printf("%s\n", command->command);
	ResponseCommands* responseCommands = malloc(sizeof(ResponseCommands));
	responseCommands->responses_length = 0;
	if (strcmp(command->command, "play_animation") == 0) {
		Animation* animation = (Animation*)command->message;
		// TODO check if animation already is available in cache
		Command* requestData = malloc(sizeof(Command));
		requestData->command = strdup("animation_data");
		requestData->message = animation;
		responseCommands->responses = malloc(2*sizeof(Command));
		responseCommands->responses[0] = requestData;
		responseCommands->responses[1] = addOkResponse();
		responseCommands->responses_length = 2;
	} else if (strcmp(command->command, "play_sequence") == 0) {
		printf("PLAY_SEQUENCE NOT IMPLEMENTED");
	} else if (strcmp(command->command, "animation_data") == 0) {
		// TODO Add ilda to cache
		//ILDA* ilda = (ILDA*)command->message;
		responseCommands->responses = malloc(sizeof(Command));
		responseCommands->responses[0] = addOkResponse();
		responseCommands->responses_length = 1;
	}
	return responseCommands;
}

socket_message* createSocketMessage(Command* command) {
	socket_message* message = malloc(sizeof(socket_message));
	if (strcmp(command->command, "animation_data") == 0) {
		Animation* animation = (Animation*)command->message;
		json_t* animationJson = json_pack("{sssisssssi}","response","animation_data","id",animation->id,"name",animation->name,"lastUpdate",animation->last_update,
						"frameRate",animation->frame_rate);
		message->content = json_dumps(animationJson,0);
		message->length = strlen(message->content);
	} else if(strcmp(command->command, "OK") == 0) {
		json_t* okJson = json_pack("{ss}","response","ok");
		message->content = json_dumps(okJson, 0);
		message->length = strlen(message->content);
	}
	return message;
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
		socket_message message;
		do {
			message = readSocketMessage(connect_d);
			if(message.length > 0) {
				printf("Serializing...%i\n", index++);
				Command* command = serialize(message.content, message.length);

				ResponseCommands* responseCommands = getResponseCommands(command);
				for(int i=0;i<responseCommands->responses_length;i++) {
					printf("Response commands: %i\n", i);
					socket_message* message = createSocketMessage(responseCommands->responses[i]);
					writeSocketMessage(connect_d, message);
				}

//				addToPlay(ilda);
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
		sleep(3);
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
