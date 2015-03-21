/*
 * json.h
 *
 *  Created on: Mar 21, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_JSON_JSON_H_
#define INCLUDE_LASFORCE_JSON_JSON_H_

#include "../../jansson/jansson.h"

int getInt(json_t* root, char* key);
char* getString(json_t* root, char* key);



#endif /* INCLUDE_LASFORCE_JSON_JSON_H_ */
