/*
 * serializeCommands.c
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */

#include "../../../include/lasforce/serialize/serializeCommands.h"
#include <errno.h>

Animation* serializeAnimation(json_t* root) {
	syslog(LOG_DEBUG, "%s", "Serializing animation.");
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
	animation->loop_count = getInt(root, "loopCount");
	syslog(LOG_DEBUG, "%s", "Serialized animation.");
	return animation;
}

AnimationRequest* serializeAnimationRequest(json_t* root) {
	syslog(LOG_DEBUG, "%s", "Serializing animation request.");
	json_t *animationInfo = json_object_get(root,"animationInfo");
	if (animationInfo == NULL) {
		syslog(LOG_DEBUG, "%s", "Need animationInfo to process AnimationRequest.");
	}
	AnimationRequest* animation_request = malloc(sizeof(AnimationRequest));
	if (animation_request == NULL) {
		perror("Can't allocate memory for AnimationRequest.");
		strerror(errno);
		exit(1);
	}
	animation_request->animation = serializeAnimation(animationInfo);
	syslog(LOG_DEBUG, "%s", "animation set.");
	syslog(LOG_DEBUG, "%s", "Serialized animation request.");
	return animation_request;
}

AnimationData* serializeAnimationDataRequest(json_t* root) {
	syslog(LOG_DEBUG, "%s", "Serializing animation data request.");
	json_t *animation = json_object_get(root, "animation");
	if (animation == NULL) {
		syslog(LOG_DEBUG, "%s", "Need animation to process AnimationDataRequest.");
	}
	json_t *animationMetaData = json_object_get(animation, "animationMetaData");
	if (animationMetaData == NULL) {
		syslog(LOG_DEBUG, "%s", "Need animationMetaData to process AnimationDataRequest.");
	}
	json_t *ilda = json_object_get(animation, "ilda");
		if (ilda == NULL) {
			syslog(LOG_DEBUG, "%s", "Need ilda to process AnimationDataRequest.");
		}
	AnimationData* animation_data_request = malloc(sizeof(AnimationData));
	if (animation_data_request == NULL) {
		perror("Can't allocate memory for AnimationDataRequest.");
		strerror(errno);
		exit(1);
	}
	animation_data_request->animation = serializeAnimation(animationMetaData);
	syslog(LOG_DEBUG, "%s", "animation set.");
	animation_data_request->ilda = serializeILDA(ilda);
	syslog(LOG_DEBUG, "%s", "ilda set.");
	syslog(LOG_DEBUG, "%s", "Serialized animation data request.");
	return animation_data_request;
}

SequenceRequest* serializeSequenceRequest(json_t* root) {
	syslog(LOG_DEBUG, "%s", "Serializing sequence request.");
	json_t *sequenceInfo = json_object_get(root, "sequenceInfo");
	if (sequenceInfo == NULL) {
		syslog(LOG_DEBUG, "%s", "Need sequenceInfo to process SequenceRequest.");
	}
	SequenceRequest* sequence_request = malloc(sizeof(SequenceRequest));
	if (sequence_request == NULL) {
		perror("Can't allocate memory for SequenceRequest.");
		strerror(errno);
		exit(1);
	}
	sequence_request->name = getString(sequenceInfo, "name");
	sequence_request->animations = NULL;
	sequence_request->animation_length = 0;
	json_t *animationList = json_object_get(sequenceInfo, "animations");
	if (animationList != NULL) {
		int animationsSize = json_array_size(animationList);
		Animation** animations = malloc(animationsSize * sizeof(Animation*));
		if (animations == NULL) {
			perror("Can't allocate memory for SequenceRequest Animations.");
			strerror(errno);
			exit(1);
		}
		int i;
		for (i = 0; i < animationsSize; i++) {
			json_t* animationJson = json_array_get(animationList, i);
			Animation *animation_request = serializeAnimation(animationJson);
			animations[i] = animation_request;
		}
		sequence_request->animations = animations;
		sequence_request->animation_length = animationsSize;
	}
	return sequence_request;
}
