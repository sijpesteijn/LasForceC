/*
 * serializeCommands.h
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_SERIALIZE_SERIALIZECOMMANDS_H_
#define INCLUDE_LASFORCE_SERIALIZE_SERIALIZECOMMANDS_H_

#include "../../lf.h"
#include "../../jansson/jansson.h"
#include "../commands.h"
#include "../json/json.h"

//Animation* serializeAnimation(json_t* root);
//Sequence* serializeSequence(json_t* root);
AnimationRequest* serializeAnimationRequest(json_t* root);
AnimationData* serializeAnimationDataRequest(json_t* root);
SequenceRequest* serializeSequenceRequest(json_t* root);


#endif /* INCLUDE_LASFORCE_SERIALIZE_SERIALIZECOMMANDS_H_ */
