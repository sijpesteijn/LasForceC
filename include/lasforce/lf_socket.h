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
} socket_message;

int createSocket();
socket_message readSocketMessage(int connect_d);
void writeSocketMessage(int connect_d, socket_message* smsg);

#endif /* INCLUDE_LASFORCE_LF_SOCKET_H_ */
