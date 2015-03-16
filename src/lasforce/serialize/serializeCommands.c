/*
 * serializeCommands.c
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */

#include "../../../include/lasforce/serialize/serializeCommands.h"
#include <errno.h>

Animation* serializeAnimation(json_t* root) {
	json_t *animationInfo = json_object_get(root,"animationInfo");
	if (animationInfo == NULL) {
		printf("Need animationInfo to play animation.\n");
	}
	json_t *id = json_object_get(animationInfo, "id");
	json_t *name = json_object_get(animationInfo, "name");
	json_t *last_update = json_object_get(animationInfo, "lastUpdate");
	json_t *frame_rate = json_object_get(animationInfo, "framerate");
	Animation* animation = malloc(sizeof(Animation));
	if (animation == NULL) {
		perror("Can't allocate memory for Annimation.");
		strerror(errno);
		exit(1);
	}
	animation->id = json_number_value(id);
	animation->name = strdup(json_string_value(name));
	animation->last_update = strdup(json_string_value(last_update));
	animation->frame_rate = json_number_value(frame_rate);
	return animation;
}

Sequence* serializeSequence(json_t* root) {
	json_t *sequenceInfo = json_object_get(root, "sequenceInfo");
	if (sequenceInfo == NULL) {
		printf("Need sequenceInfo to play sequence");
	}
	Sequence* sequence = malloc(sizeof(Sequence));
	json_t *name = json_object_get(sequenceInfo, "name");
	sequence->name = strdup(json_string_value(name));
	json_t *animationList = json_object_get(root, "animations");
	if (animationList != NULL) {
		int animationsSize = json_array_size(animationList);
		Animation** animations = malloc(animationsSize * sizeof(Animation*));
		int i;
		for (i = 0; i < animationsSize; i++) {
			json_t* animationJson = json_array_get(animationList, i);
			Animation *animation = serializeAnimation(animationJson);
			animations[i] = animation;
		}
		sequence->animations = animations;
	}
	return sequence;
}



