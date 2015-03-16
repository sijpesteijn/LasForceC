/*
 * ILDA.h
 *
 *  Created on: Feb 23, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_ILDA_H_
#define INCLUDE_ILDA_H_

typedef struct {
	int dirty;
	int maxDepth;
	int maxWidth;
	int maxHeight;
	int minDepth;
	int minWidth;
	int minHeight;
	int nrOfFrames;
} ILDAHeader;

typedef struct {
	const char* protocol;
	int formatCode;
	const char* paletteName;
	const char* companyName;
	int totalColors;
	int paletteNumber;
	int scannerHead;
} ColorHeader;

typedef struct {
	int red1;
	int red2;
	int green1;
	int green2;
	int blue1;
	int blue2;
	int code;
} ColorData;

typedef struct {
	int x;
	int y;
	int z;
	ColorData *colorData;
	int blanked;
	int endImageData;
} CoordinateData;

typedef struct {
	const char* protocol;
	int threeD;
	const char* frameName;
	const char* companyName;
	int totalPoints;
	int frameNumber;
	int scannerHead;
	CoordinateData **coordinateDatas;
	int totalCoordinateDatas;
} CoordinateHeader;

typedef struct {
	ILDAHeader *ildaHeader;
	ColorHeader *colorHeader;
	ColorData **colorDatas;
	int totalColorDatas;
	CoordinateHeader **coordinateHeaders;
	int totalCoordinateHeaders;
} ILDA;

void destroyIlda(ILDA ilda);

#endif /* INCLUDE_ILDA_H_ */
