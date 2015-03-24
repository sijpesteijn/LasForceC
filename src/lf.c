/*
 * LasForceC.c
 *
 *  Created on: Feb 23, 2015
 *      Author: gijs
 */

#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "../include/lf.h"
#include "../include/lasforce/ilda/ILDA.h"
#include "../include/lasforce/serialize/serialize.h"
#include "../include/lasforce/ilda/deserialize.h"
#include "../include/lasforce/lf_socket.h"

Queue* playQueue = NULL;
Queue* currentAnimation;
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

int inCache(Animation* animation) {
	int i=0;
	pthread_mutex_lock(&cache_lock);
	for(i=0;i<ANIMATION_CACHE_SIZE;i++) {
		if (animation_cache[i] != NULL && animation_cache[i]->animation->id == animation->id) {
			syslog(LOG_DEBUG, "%s", "inCache: IN cache");
			pthread_mutex_unlock(&cache_lock);
			return 1;
		}
	}
	syslog(LOG_DEBUG, "%s", "inCache: NOT in cache");
	pthread_mutex_unlock(&cache_lock);
	return 0;
}

int addToAnimationDataCache(AnimationData* animation_data) {
	int i=0;
	pthread_mutex_lock(&cache_lock);
	for(i=0;i<ANIMATION_CACHE_SIZE;i++) {
		if (animation_cache[i] == NULL) {
			animation_cache[i] = animation_data;
			syslog(LOG_DEBUG, "%s", "addToAnimationDataCache: ilda added.");
			pthread_mutex_unlock(&cache_lock);
			return 1;
		}
	}
	syslog(LOG_ALERT, "%s", "CACHE IS FULL. Implement fifo.");
	pthread_mutex_unlock(&cache_lock);
	return 0;
}

int addToQueue(Command* scene) {
	pthread_mutex_lock(&queue_lock);
	Queue *nextElement = malloc(sizeof(Queue));
	if (nextElement == NULL) {
		perror("Can't allocate memory for Queue.");
		strerror(errno);
		exit(1);
	}
	nextElement->scene = scene->message;
	nextElement->next = NULL;
	if (playQueue == NULL) {
		playQueue = nextElement;
	} else {
		Queue* next = playQueue;
		while(next->next != NULL) {
			next = next->next;
		}
		next->next = nextElement;
	}
	pthread_mutex_unlock(&queue_lock);
	return 0;
}

Command* getOkResponse() {
	Command* command = malloc(sizeof(Command));
	command->command = "OK";
	command->command_length = 2;
	command->next = NULL;
	syslog(LOG_DEBUG, "%s", "OkResponse created.");
	return command;
}

Command* getErrorResponse(char* message) {
	Command* command = malloc(sizeof(Command));
	command->command = "ERROR";
	command->command_length = 5;
	command->next = NULL;
	syslog(LOG_DEBUG, "%s", "ErrorResponse created.");
	return command;
}

Command* getResponseCommands(Command* command) {
	syslog(LOG_DEBUG, "%s", "getResponseCommands: Start.");
	if (strcmp(command->command, "play_animation") == 0) {
		syslog(LOG_DEBUG, "%s", "getResponseCommands: play animation");
		AnimationRequest* animationRequest = (AnimationRequest*)command->message;
		Animation* animation = animationRequest->animation;
		if (inCache(animation)) {
			return getOkResponse();
		} else {
			syslog(LOG_DEBUG, "%s", "getResponseCommands: creating animation data response.");
			Command* requestData = malloc(sizeof(Command));
			requestData->command = "animation_data";
			requestData->command_length = 14;
			requestData->next = NULL;
			syslog(LOG_DEBUG, "%s", "getResponseCommands: copying animation data.");
			Animation* animationData = malloc(sizeof(Animation));
			animationData->id = animation->id;
			animationData->name = strdup(animation->name);
			animationData->last_update = strdup(animation->last_update);
			animationData->frame_rate = animation->frame_rate;
			animationData->loop_count = animation->loop_count;
			syslog(LOG_DEBUG, "%s", "getResponseCommands: finished copying animation data.");
			requestData->message = animationData;
			syslog(LOG_DEBUG, "%s", "getResponseCommands: finished animation data response.");
			return requestData;
		}
	} else if (strcmp(command->command, "play_sequence") == 0) {
		syslog(LOG_DEBUG, "%s", "getResponseCommands: play sequence");
		SequenceRequest* sequenceRequest = (SequenceRequest*)command->message;
		int i=0;
		Command* response = NULL;
		for(i=0;i<sequenceRequest->animation_length;i++) {
			Animation* animation = sequenceRequest->animations[i];
			if(inCache(animation) == 0) {
				syslog(LOG_DEBUG, "%s", "getResponseCommands: creating animation data response.");
				Command* requestData = malloc(sizeof(Command));
				requestData->command = "animation_data";
				requestData->command_length = 14;
				requestData->next = NULL;
				syslog(LOG_DEBUG, "%s", "getResponseCommands: copying animation data.");
				Animation* animationData = malloc(sizeof(Animation));
				animationData->id = animation->id;
				animationData->name = strdup(animation->name);
				animationData->last_update = strdup(animation->last_update);
				animationData->frame_rate = animation->frame_rate;
				animationData->loop_count = animation->loop_count;
				syslog(LOG_DEBUG, "%s", "getResponseCommands: finished copying animation data.");
				requestData->message = animationData;
				syslog(LOG_DEBUG, "%s", "getResponseCommands: finished animation data response.");
				if (response == NULL) {
					response = requestData;
				} else {
					Command* next = response->next;
					while(next->next != NULL) {
						next = next->next;
					}
					next->next = requestData;
				}
			}
		}
		if (response == NULL) {
			return getOkResponse();
		}
		return response;
	} else if (strcmp(command->command, "animation_data") == 0) {
		syslog(LOG_DEBUG, "%s", "getResponseCommands: animation data");
		AnimationData* animation_data = (AnimationData*)command->message;
		if (addToAnimationDataCache(animation_data) == 0) {
			return getErrorResponse("001-ILDA CACHE IS FULL.");
		}
		return getOkResponse();
	}
	syslog(LOG_ALERT, "%s", "getResponseCommands: euh. This should not happen");
	return NULL;
}

socket_message* createSocketMessage(Command* command) {
	socket_message* message = malloc(sizeof(socket_message));
	if (strcmp(command->command, "animation_data") == 0) {
		Animation* animation = (Animation*)command->message;
//		json_t* animationsJson = json_array();
//		Command* next = command;
//		while (next != NULL) {
			json_t* animationJson = json_pack("{sssisssssisi}","response","animation_data","id",animation->id,"name",animation->name,"lastUpdate",animation->last_update,
							"frameRate",animation->frame_rate,"loopCount", animation->loop_count);
//			json_array_append(animationsJson, animationJson);
//			next = next->next;
//		}
//		char* msg = concat("{\"response\": \"list\", \"elements\" : ", json_dumps(animationsJson,0));
//		msg = concat(msg,"}");
		message->content = json_dumps(animationJson,0); //msg;
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
	syslog(LOG_INFO, "%s", "Message handler started.\n");
	int listener_d = createSocket();
	struct sockaddr_storage client_addr;
	unsigned int address_size = sizeof(client_addr);
	while(1) {
		syslog(LOG_INFO, "%s", "Waiting for client connections....\n");
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
				syslog(LOG_INFO, "Serializing...%i\n", index++);
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
				addToQueue(command);
				freeCommand(command);
				freeCommand(response);
				freeSocketMessage(responseMessage);
			}
		} while (message->more);
		close(connect_d);
	}
	return NULL;
}

void* playScene(void* param) {
	AnimationData* animationData = (AnimationData*) currentAnimation->scene;
	int loop_count = animationData->animation->loop_count;
	float fps = 1.0f / animationData->animation->frame_rate;
	printf("LOOP %f\n", fps);
//	struct timespec tim, tim2;
//	tim.tv_sec = 1;
//	tim.tv_nsec = 500;
	syslog(LOG_INFO, "playScene: playing %s %i times\n", animationData->animation->name, loop_count);
	while(loop_count > 0) {
//		ILDA* ilda = animationData->ilda;
//		int i;
//		for(i=0;i<ilda->totalCoordinateHeaders;i++) {
//			CoordinateHeader* coordinateHeader = ilda->coordinateHeaders[i];
//			syslog(LOG_INFO, "playScene: playing %i", i);
			sleep(2);
//		}
		loop_count--;
	}

	currentAnimation = NULL;
	return NULL;
}

void* startQueueMonitor(void* param) {
	syslog(LOG_INFO, "%s", "ILDA Player started.\n");
	while(1) {
		int total = 0;
		Queue* next = playQueue;
		while(next != NULL) {
			total++;
			next = next->next;
		}
		syslog(LOG_DEBUG, "startQueueMonitor: Found %i elements on the queue\n", total);
		pthread_t playSceneThread;
		void *result;
		while(playQueue != NULL && currentAnimation == NULL) {
			if (playSceneThread != NULL)
				pthread_join(playSceneThread, &result);
			syslog(LOG_DEBUG, "startQueueMonitor: starting next animation.");
			pthread_mutex_lock(&queue_lock);
			currentAnimation = playQueue;
			playQueue = currentAnimation->next;
			pthread_mutex_unlock(&queue_lock);
			if (pthread_create(&playSceneThread,NULL, playScene, NULL))
				perror("Can't create message_handler thread");
		}
		sleep(1);
	}
	return NULL;
}

int main(int argc, const char * argv[]) {
	openlog ("LasForce", LOG_PID | LOG_CONS | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL0);
	syslog (LOG_INFO, "%s", "Starting LasForce");

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
	closelog();
	return 0;
}
