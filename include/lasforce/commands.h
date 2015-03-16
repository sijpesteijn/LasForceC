/*
 * messages.h
 *
 *  Created on: Mar 14, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_ILDA_MESSAGES_H_
#define INCLUDE_LASFORCE_ILDA_MESSAGES_H_

typedef struct {
	const char* command;
	int command_length;
	void* message;
} Command;

typedef struct {
	Command** responses;
	int responses_length;
} ResponseCommands;

typedef struct {
	int id;
	const char* name;
	const char* last_update;
	int frame_rate;
} Animation;

typedef struct {
	const char* name;
	Animation** animations;
} Sequence;



#endif /* INCLUDE_LASFORCE_ILDA_MESSAGES_H_ */
