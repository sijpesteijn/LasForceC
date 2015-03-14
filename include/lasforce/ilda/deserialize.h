/*
 * ILDADeserialize.h
 *
 *  Created on: Feb 24, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_ILDADESERIALIZE_H_
#define INCLUDE_ILDADESERIALIZE_H_

#include "../../lf.h"

char* getTabs(int indent) {
	   char* tabs = "";
	   char* tab = "\t";
	   int i;
	   for(i=0;i<indent;i++) {
		   tabs = concat(tabs, tab);
	   }
	   return tabs;
}

char* deserializeColorHeader(ColorHeader *colorHeader, int indent) {
	char *tabs = getTabs(indent);
	char *fieldTabs = getTabs(indent+1);
	char *json;
	asprintf(&json,
			"\"colorHeader\":\n%s{\n%s\"scannerHead\":%i,\n%s\"paletteNumber\":%i,\n%s\"totalColors\":%i,\n%s\"companyName\":\"%s\",\n%s\"paletteName\":\"%s\",\n%s\"formatCode\":%i,\n%s\"protocol\":\"%s\"\n%s}",
			tabs, fieldTabs, colorHeader->scannerHead, fieldTabs,  colorHeader->paletteNumber, fieldTabs,  colorHeader->totalColors, fieldTabs,  colorHeader->companyName, fieldTabs,
			colorHeader->paletteName, fieldTabs, colorHeader->formatCode, fieldTabs, colorHeader->protocol, tabs);

	if (debug) {
		printf("Color header:\n\tscanner head: %i\n", colorHeader->scannerHead);
		printf("\tPalette nr: %i\n", colorHeader->paletteNumber);
		printf("\tTotal colors: %i\n", colorHeader->totalColors);
		printf("\tCompany name: %s\n", colorHeader->companyName);
		printf("\tPalette name: %s\n", colorHeader->paletteName);
		printf("\tFormat code: %i\n", colorHeader->formatCode);
		printf("\tProtocol: %s\n", colorHeader->protocol);
	}
	return json;
}

char* deserializeIldaHeader(ILDAHeader *ildaHeader, int indent) {
	char *tabs = getTabs(indent);
	char *fieldTabs = getTabs(indent+1);
	char *json;
	asprintf(&json, "%s\"header\":{\n%s\"dirty\":%i,\n%s\"maxDepth\":%i,\n%s\"maxWidth\":%i,\n%s\"maxHeight\":%i,\n%s\"minDepth\":%i,\n%s\"minWidth\":%i,\n%s\"minHeight\":%i,\n%s\"nrOfFrames\":%i\n%s}",
			tabs, fieldTabs, ildaHeader->dirty, fieldTabs, ildaHeader->maxDepth, fieldTabs, ildaHeader->maxWidth, fieldTabs, ildaHeader->maxHeight, fieldTabs, ildaHeader->minDepth, fieldTabs, ildaHeader->minWidth, fieldTabs,
			ildaHeader->minHeight, fieldTabs, ildaHeader->nrOfFrames, tabs);
	if (debug) {
		printf("ILDA Header:\n\tDirty: %i\n", ildaHeader->dirty);
		printf("\tMax Depth: %i\n", ildaHeader->maxDepth);
		printf("\tMax Width: %i\n", ildaHeader->maxWidth);
		printf("\tMax Height: %i\n", ildaHeader->maxHeight);
		printf("\tMin Depth: %i\n", ildaHeader->minDepth);
		printf("\tMin Width: %i\n", ildaHeader->minWidth);
		printf("\tMin Height: %i\n", ildaHeader->minHeight);
		printf("\tNr of frames: %i\n", ildaHeader->nrOfFrames);
	}
	return json;
}

char* deserializeColorData(ColorData *colorData, int indent) {
	char *tabs = getTabs(indent);
	char *fieldTabs = getTabs(indent+1);
	char *json;
	asprintf(&json,
			"{\n%s\"red1\":%i,\n%s\"red2\":%i,\n%s\"green1\":%i,\n%s\"green2\":%i,\n%s\"blue1\":%i,\n%s\"blue2\":%i,\n%s\"code\":%i\n%s}",
			fieldTabs, colorData->red1, fieldTabs, colorData->red2, fieldTabs, colorData->green1, fieldTabs, colorData->green2, fieldTabs, colorData->blue1, fieldTabs, colorData->blue2, fieldTabs, colorData->code, tabs);
	if (0 && debug) {
		printf("Color Data\n\tRed1: %i\n", colorData->red1);
		printf("\tRed2: %i\n", colorData->red2);
		printf("\tGreen1: %i\n", colorData->green1);
		printf("\tGreen2: %i\n", colorData->green2);
		printf("\tBlue1: %i\n", colorData->blue1);
		printf("\tBlue2: %i\n", colorData->blue2);
		printf("\tCode: %i\n", colorData->code);
	}

	return json;
}

char* deserializeCoordinateData(CoordinateData *coordinateData, int indent) {
	const char *tabs = getTabs(indent);
	const char *fieldTabs = getTabs(indent + 1);
	char *colorDataJson = "";
	colorDataJson = concat(colorDataJson, deserializeColorData(coordinateData->colorData,indent+1));
	char *json;
	asprintf(&json,
			"%s{\n%s\"x\":%i,\n%s\"y\":%i,\n%s\"z\":%i,\n%s\"blanked\":%i,\n%s\"endImageData\":%i,\n%s\"colorData\":%s\n%s}",
			tabs,fieldTabs, coordinateData->x, fieldTabs, coordinateData->y, fieldTabs, coordinateData->z, fieldTabs, coordinateData->blanked, fieldTabs, coordinateData->endImageData, fieldTabs, colorDataJson, tabs);
	if (0 && debug) {
		printf("Coordinate data:\n\tx: %i\n", coordinateData->x);
		printf("\ty: %i\n", coordinateData->y);
		printf("\tz: %i\n", coordinateData->z);
		printf("\tblanked: %i\n", coordinateData->blanked);
		printf("\tend image data: %i\n", coordinateData->endImageData);
	}
	return json;
}

char* deserializeCoordinateHeader(CoordinateHeader *coordinateHeader, int indent) {
	char *tabs = getTabs(indent);
	char *fieldTabs = getTabs(indent + 1);

	char *coordinateDataJson = getTabs(indent - 1);
	coordinateDataJson = concat(coordinateDataJson, "\"coordinateDatas\":[\n");
	int i = 0;
	for(i=0;i<coordinateHeader->totalCoordinateDatas;i++) {
		coordinateDataJson = concat(coordinateDataJson, deserializeCoordinateData(coordinateHeader->coordinateDatas[i], indent + 1));
		if (i < coordinateHeader->totalCoordinateDatas-1) {
			coordinateDataJson = concat(coordinateDataJson, ",");
		}
		coordinateDataJson = concat(coordinateDataJson, "\n");
	}
	coordinateDataJson = concat(coordinateDataJson, fieldTabs);
	coordinateDataJson = concat(coordinateDataJson,"]\n");

	char *json = "";
	asprintf(&json,
			"%s{\n%s\"protocol\":\"%s\",\n%s\"threeD\":%i,\n%s\"frameName\":\"%s\",\n%s\"companyName\":\"%s\",\n%s\"totalPoints\":%i,\n%s\"frameNumber\":%i,\n%s\"scannerHead\":%i,\n%s%s%s}",
			tabs, fieldTabs, coordinateHeader->protocol, fieldTabs, coordinateHeader->threeD, fieldTabs, coordinateHeader->frameName, fieldTabs, coordinateHeader->companyName, fieldTabs, coordinateHeader->totalPoints, fieldTabs, coordinateHeader->frameNumber, fieldTabs, coordinateHeader->scannerHead, tabs, coordinateDataJson, tabs);
	if(debug) {
		printf("Coordinate Header:\n\tProtocol: %s\n", coordinateHeader->protocol);
		printf("\tThreeD: %i\n", coordinateHeader->threeD);
		printf("\tFrame name: %s\n", coordinateHeader->frameName);
		printf("\tCompany name: %s\n", coordinateHeader->companyName);
		printf("\tTotal points: %i\n", coordinateHeader->totalPoints);
		printf("\tFrame nr: %i\n", coordinateHeader->frameNumber);
		printf("\tScanner head: %i\n", coordinateHeader->scannerHead);
	}

	return json;
}

char* deserialize(ILDA ilda) {
	char *output = "";
	output = concat(output, "{\n");
	output = concat(output, deserializeIldaHeader(ilda.ildaHeader, 1));
	output = concat(output, ",\n");
	output = concat(output, deserializeColorHeader(ilda.colorHeader, 1));
	output = concat(output, ",\n");
	output = concat(output, "\"colorData\":[\n");
	if (ilda.totalColorDatas > 0) {
		int i;
		for (i = 0; i < ilda.totalColorDatas; i++) {
		output = concat(output, deserializeColorData(ilda.colorDatas[i], 1));
			if (i < ilda.totalColorDatas-1) {
				output = concat(output, ",");
			}
			output = concat(output, "\n");
		}
	}
	output = concat(output, "],\n");
	output = concat(output, "\"coordinateHeaders\":[\n");
	if (ilda.totalCoordinateHeaders > 0) {
		int i;
		for (i = 0; i < ilda.totalCoordinateHeaders; i++) {
			deserializeCoordinateHeader(ilda.coordinateHeaders[i], 1);
		output = concat(output, deserializeCoordinateHeader(ilda.coordinateHeaders[i], 1));
			if (i < ilda.totalCoordinateHeaders-1) {
				output = concat(output, ",");
			}
			output = concat(output, "\n");
		}
	}
	output = concat(output, "]\n");
	output = concat(output, "}");
	return output;

}

#endif /* INCLUDE_ILDADESERIALIZE_H_ */
