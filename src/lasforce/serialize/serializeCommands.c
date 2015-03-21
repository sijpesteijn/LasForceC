/*
 * serializeCommands.c
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */

#include "../../../include/lasforce/serialize/serializeCommands.h"
#include <errno.h>

Animation* serializeAnimation(json_t* root) {
	gol("Serializing animation.",1);
	Animation* animation = malloc(sizeof(Animation));
	if (animation == NULL) {
		perror("Can't allocate memory for Animation.");
		strerror(errno);
		exit(1);
	}
	animation->id = getInt(root, "id");
	animation->name = getString(root, "name");
	animation->last_update = getString(root, "lastUpdate");
	animation->frame_rate = getInt(root, "frameRate");
	gol("Serialized animation.",1);
	return animation;
}

AnimationRequest* serializeAnimationRequest(json_t* root) {
	gol("Serializing animation request.",1);
	json_t *animationInfo = json_object_get(root,"animationInfo");
	if (animationInfo == NULL) {
		gol("Need animationInfo to process AnimationRequest.",1);
	}
	AnimationRequest* animation_request = malloc(sizeof(AnimationRequest));
	if (animation_request == NULL) {
		perror("Can't allocate memory for AnimationRequest.");
		strerror(errno);
		exit(1);
	}
	animation_request->animation = serializeAnimation(animationInfo);
	gol("animation set.",1);
	gol("Serialized animation request.",1);
	return animation_request;
}

AnimationData* serializeAnimationDataRequest(json_t* root) {
	gol("Serializing animation data request.",1);
	json_t *animationDataRequest = json_object_get(root, "animationMetaData");
	if (animationDataRequest == NULL) {
		gol("Need animationData to process AnimationDataRequest.",1);
	}
	AnimationData* animation_data_request = malloc(sizeof(AnimationData));
	if (animation_data_request == NULL) {
		perror("Can't allocate memory for AnimationDataRequest.");
		strerror(errno);
		exit(1);
	}
	animation_data_request->animation = serializeAnimation(animationDataRequest);
	gol("animation set.",1);
	animation_data_request->ilda = serializeILDA(animationDataRequest);
	gol("ilda set.",1);
	gol("Serialized animation data request.",1);
	return animation_data_request;
}

SequenceRequest* serializeSequenceRequest(json_t* root) {
	gol("Serializing sequence request.",1);
	json_t *sequenceInfo = json_object_get(root, "sequenceInfo");
	if (sequenceInfo == NULL) {
		gol("Need sequenceInfo to process SequenceRequest.",1);
	}
	SequenceRequest* sequence_request = malloc(sizeof(SequenceRequest));
	if (sequence_request == NULL) {
		perror("Can't allocate memory for SequenceRequest.");
		strerror(errno);
		exit(1);
	}
	sequence_request->name = getString(sequenceInfo, "name");
	json_t *animationList = json_object_get(root, "animations");
	if (animationList != NULL) {
		int animationsSize = json_array_size(animationList);
		AnimationRequest** animation_requests = malloc(animationsSize * sizeof(AnimationRequest*));
		if (animation_requests == NULL) {
			perror("Can't allocate memory for SequenceRequest AnimationRequests.");
			strerror(errno);
			exit(1);
		}
		int i;
		for (i = 0; i < animationsSize; i++) {
			json_t* animationJson = json_array_get(animationList, i);
			AnimationRequest *animation_request = serializeAnimationRequest(animationJson);
			animation_requests[i] = animation_request;
		}
		sequence_request->animation_requests = animation_requests;
		sequence_request->animation_requests_length = animationsSize;
	}
	return sequence_request;
}



