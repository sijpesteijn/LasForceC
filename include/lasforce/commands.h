/*
 * messages.h
 *
 *  Created on: Mar 14, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_ILDA_MESSAGES_H_
#define INCLUDE_LASFORCE_ILDA_MESSAGES_H_

#include "serialize/serializeILDA.h"

typedef struct Command {
	char* command;
	int command_length;
	void* message;
	struct Command* next;
} Command;

typedef struct {
	int id;
	char* name;
	char* last_update;
	int frame_rate;
} Animation;

typedef struct {
	Animation* animation;
} AnimationRequest;

typedef struct {
	char* name;
	AnimationRequest** animation_requests;
	int animation_requests_length;
} SequenceRequest;

typedef struct {
	Animation* animation;
	ILDA* ilda;
} AnimationData;

typedef struct {
	char* name;
	Animation** animations;
	int animations_length;
} Sequence;

typedef struct Queue {
	int loop;
	void* scene;
	struct Queue* next;
} Queue;

#endif /* INCLUDE_LASFORCE_ILDA_MESSAGES_H_ */
