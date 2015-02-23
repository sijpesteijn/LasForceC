/*
 * LasForceC.c
 *
 *  Created on: Feb 23, 2015
 *      Author: gijs
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "../include/ILDA.h"
#include "../include/LasForce.h"

void testILDA() {
	ColorData colorData1 = {255,0,255,0,255,0,12};
	ColorData colorData2 = {254,0,254,0,254,0,11};
	printf("ColorData red1: %i\n", colorData1.red1);
	printf("ColorData red2: %i\n", colorData1.red2);
	printf("ColorData blue1: %i\n", colorData1.blue1);
	printf("ColorData blue2: %i\n", colorData1.blue2);
	printf("ColorData green1: %i\n", colorData1.green1);
	printf("ColorData green2: %i\n", colorData1.green2);
	printf("ColorData code: %i\n", colorData1.code);

	ColorData colorDatas[2];
	colorDatas[0] = colorData1;
	colorDatas[1] = colorData2;
	CoordinateData coordinateData = {0,12,35, colorDatas, 0,0};
	coordinateData.totalColorDatas = 2;

	printf("Nr: %i\n", coordinateData.totalColorDatas);
	printf("CoordinateData: %i\n", coordinateData.colorDatas[1].blue1);
}

int main(int argc, const char * argv[]) {
	printf("Starting LasForce.\n");

	// testILDA();

	int listener_d = createSocket();
	printf("Waiting for client connections....\n");
	handleSocketsRequestMessages(listener_d);

	return 0;
}
