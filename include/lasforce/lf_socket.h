/*
 * lf_socket.h
 *
 *  Created on: Feb 24, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_LF_SOCKET_H_
#define INCLUDE_LASFORCE_LF_SOCKET_H_

#include "../../include/lf.h"

typedef struct {
	char *content;
	unsigned long length;
	int more;
} ilda_message;

int createSocket();
ilda_message readSocketMessage(int connect_d);

#endif /* INCLUDE_LASFORCE_LF_SOCKET_H_ */
