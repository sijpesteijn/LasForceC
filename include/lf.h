/*
 * LasForce.h
 *
 *  Created on: Feb 23, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_H_
#define INCLUDE_LASFORCE_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

char* concat(char *s1, char *s2);
void error(const char *msg);
void gol(const char *msg, int level);

#endif /* INCLUDE_LASFORCE_H_ */
