/*
 * serialize.c
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */


#include "../../../include/lf.h"
#include "../../../include/jansson/jansson.h"
#include "../../../include/lasforce/serialize/serializeCommands.h"
#include "../../../include/lasforce/serialize/serializeILDA.h"
#include "../../../include/lasforce/commands.h"
#include <errno.h>

Command* getCommand(json_t* root) {
	json_t *commandJson = json_object_get(root,"command");
	if (commandJson == NULL) {
		printf("Need command object to process.");
		return NULL;
	}
	Command* command = malloc(sizeof(Command));
	if (command == NULL) {
		perror("Can't allocate memory for Command.");
		strerror(errno);
		exit(1);
	}

	command->command = strdup(json_string_value(commandJson));
	printf("Cmd: %s.\n", command->command);
	if (strcmp(command->command, "play_animation") == 0) {
		command->message = serializeAnimation(root);
	} else if (strcmp(command->command, "play_sequence") == 0) {
		command->message = serializeSequence(root);
	} else if (strcmp(command->command, "animation_date") == 0) {
		command->message = serializeILDA(root);
	}
	return command;
}

Command* serialize(char* smsg, int smsgl) {
	json_t* root;
	json_error_t error;

	root = json_loadb(smsg, smsgl, 0, &error);
	if (!root) {
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
	}
	if (!json_is_object(root)) {
		fprintf(stderr, "error: commit data is not an object\n");
		json_decref(root);
	}
	Command* command = getCommand(root);
	json_decref(root);
	return command;
}

