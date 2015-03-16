/*
 * ILDASerialize.h
 *
 *  Created on: Feb 24, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_ILDASERIALIZE_H_
#define INCLUDE_ILDASERIALIZE_H_

#include "../../jansson/jansson.h"
#include "../commands.h"

Command* getCommand(json_t* root);
Command* serialize(char* smsg, int smsgl);

#endif /* INCLUDE_ILDASERIALIZE_H_ */
