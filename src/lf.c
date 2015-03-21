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

Queue* playQueue = NULL;
#define ANIMATION_CACHE_SIZE 100
AnimationData** animation_cache;
pthread_mutex_t cache_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

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

void gol(const char *msg, int level) {
	if (level == 1)
		printf("%s\n", msg);
}

int inCache(Animation* animation) {
	int i=0;
	pthread_mutex_lock(&cache_lock);
	for(i=0;i<ANIMATION_CACHE_SIZE;i++) {
		if (animation_cache[i] != NULL && animation_cache[i]->animation->id == animation->id) {
			gol("inCache: IN cache",1);
			pthread_mutex_unlock(&cache_lock);
			return 1;
		}
	}
	gol("inCache: NOT in cache",1);
	pthread_mutex_unlock(&cache_lock);
	return 0;
}

int addToAnimationDataCache(AnimationData* animation_data) {
	int i=0;
	pthread_mutex_lock(&cache_lock);
	for(i=0;i<ANIMATION_CACHE_SIZE;i++) {
		if (animation_cache[i] == NULL) {
			animation_cache[i] = animation_data;
			gol("addToAnimationDataCache: ilda added.",2);
			pthread_mutex_unlock(&cache_lock);
			return 1;
		}
	}
	gol("CACHE IS FULL. Implement fifo.",1);
	pthread_mutex_unlock(&cache_lock);
	return 0;
}

int addToQueue(Queue* queue, Command* command) {
	if(queue == NULL) {
		queue = malloc(sizeof(Queue));
		if (queue == NULL) {
			perror("Can't allocate memory for Queue.");
			strerror(errno);
			exit(1);
		}
		queue->scene = command;
		queue->next = NULL;
		gol("addToQueue: scene added.",1);
		return 0;
	}
	return addToQueue(queue->next, command);
}

Command* getOkResponse() {
	Command* command = malloc(sizeof(Command));
	command->command = "OK";
	command->command_length = 2;
	command->next = NULL;
	gol("OkResponse created.",1);
	return command;
}

Command* getErrorResponse(char* message) {
	Command* command = malloc(sizeof(Command));
	command->command = "ERROR";
	command->command_length = 5;
	command->next = NULL;
	gol("ErrorResponse created.",1);
	return command;
}

Command* getResponseCommands(Command* command) {
	gol("getResponseCommands: Start.",1);
	if (strcmp(command->command, "play_animation") == 0) {
		gol("getResponseCommands: play animation",2);
		AnimationRequest* animationRequest = (AnimationRequest*)command->message;
		Animation* animation = animationRequest->animation;
		if (inCache(animation)) {
			return getOkResponse();
		} else {
			gol("getResponseCommands: creating animation data response.",2);
			Command* requestData = malloc(sizeof(Command));
			requestData->command = "animation_data";
			requestData->command_length = 14;
			requestData->next = NULL;
			gol("getResponseCommands: copying animation data.",2);
			Animation* animationData = malloc(sizeof(Animation));
			animationData->id = animation->id;
			animationData->name = strdup(animation->name);
			animationData->last_update = strdup(animation->last_update);
			animationData->frame_rate = animation->frame_rate;
			gol("getResponseCommands: finished copying animation data.",2);
			requestData->message = animationData;
			gol("getResponseCommands: finished animation data response.",2);
			return requestData;
		}
	} else if (strcmp(command->command, "play_sequence") == 0) {
		gol("getResponseCommands: play sequence",2);
	} else if (strcmp(command->command, "animation_data") == 0) {
		gol("getResponseCommands: animation data",2);
		AnimationData* animation_data = (AnimationData*)command->message;
		if (addToAnimationDataCache(animation_data) == 0) {
			return getErrorResponse("001-ILDA CACHE IS FULL.");
		}
		return getOkResponse();
	}
	gol("getResponseCommands: euh. This should not happen",1);
	return NULL;
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

void freeSocketMessage(socket_message* socketMessage) {
	if (socketMessage != NULL) {
		free(socketMessage->content);
		free(socketMessage);
	}
}

void freeCommand(Command* command) {
	if (command != NULL) {
		if (command->next != NULL) {
			printf("NEXT %s\n", command->command);
			freeCommand(command->next);
		}
		if (strcmp(command->command, "play_animation") == 0) {
//			Animation* animation = command->message;
//			free(animation->name);
//			free(animation->last_update);
//			free(animation);
		} else if(strcmp(command->command, "play_sequence") == 0) {
		} else if(strcmp(command->command, "animation_data") == 0) {
//			Animation* animation = command->message;
//			free(animation->name);
//			free(animation->last_update);
//			free(animation);
		}

//		free(command->command);
//		free(command);
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
		socket_message* message = NULL;
		do {
			freeSocketMessage(message);
			message = readSocketMessage(connect_d);
			if(message->length > 0) {
				printf("Serializing...%i\n", index++);
				Command *command = serialize(message->content, message->length);
				Command *response = getResponseCommands(command);
				socket_message *responseMessage = createSocketMessage(response);
				writeSocketMessage(connect_d, responseMessage);

				// If response was not OK response, start dialog.
				while(strcmp(response->command, "OK") != 0) {
					socket_message* dialog_msg = readSocketMessage(connect_d);
					if (dialog_msg->length > 0) {
						freeCommand(response); // free old response
						Command *dialog_cmd = serialize(dialog_msg->content, dialog_msg->length);
						response = getResponseCommands(dialog_cmd);
						socket_message *responseMessage = createSocketMessage(response);
						writeSocketMessage(connect_d, responseMessage);
					}
				}
				printf("COMMAND: %s\n", command->command);
				pthread_mutex_lock(&queue_lock);
				addToQueue(playQueue, command);
				pthread_mutex_unlock(&queue_lock);
				freeCommand(command);
				freeCommand(response);
				freeSocketMessage(responseMessage);
			}
		} while (message->more);
		close(connect_d);
	}
	return NULL;
}

void* startQueueMonitor(void* param) {
	printf("ILDA Player started.\n");
	while(1) {
		sleep(3);
		printf("startQueueMonitor: Checking table\n");
		pthread_mutex_lock(&queue_lock);
		if (playQueue != NULL) {
			gol("startQueueMonitor: Found elmenet on the queue.",1);
		}
		pthread_mutex_unlock(&queue_lock);
	}
	return NULL;
}

int main(int argc, const char * argv[]) {
	printf("Starting LasForce.\n");

	playQueue = NULL;
	animation_cache = malloc(ANIMATION_CACHE_SIZE*sizeof(ILDA));
	pthread_t message_handler, ilda_player;
	if (pthread_create(&message_handler,NULL, handleMessages, NULL))
		perror("Can't create message_handler thread");
	if (pthread_create(&ilda_player, NULL, startQueueMonitor, NULL))
		perror("Can't create ilda_player thread");
	void* result;
	pthread_join(message_handler, &result);
	pthread_join(ilda_player, &result);
	return 0;
}
