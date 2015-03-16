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

Animation* serializeAnimation(json_t* root);
Sequence* serializeSequence(json_t* root);


#endif /* INCLUDE_LASFORCE_SERIALIZE_SERIALIZECOMMANDS_H_ */
