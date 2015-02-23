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
	char protocol[5];
	int formatCode;
	char paletteName[9];
	char companyName[9];
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
	ColorData *colorDatas;
	int totalColorDatas;
	int blanked;
	int endImageData;
} CoordinateData;

typedef struct {
	char protocol[5];
	int threeD;
	char frameName[9];
	char companyName[9];
	int totalPoints;
	int frameNumber;
	int totalFrames;
	int scannerHead;
	CoordinateData *coordinateDatas;
	int totalCoordinateDatas;
} CoordinateHeader;

typedef struct {
	ILDAHeader *ildaHeader;
	ColorHeader *colorHeader;
	ColorData *colorDatas;
	int totalColorDatas;
	CoordinateHeader *coordinateHeaders;
	int totalCoordinateHeaders;
} ILDA;



#endif /* INCLUDE_ILDA_H_ */
