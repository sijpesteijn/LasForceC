/*
 * ILDASerialize.h
 *
 *  Created on: Feb 24, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_ILDASERIALIZE_H_
#define INCLUDE_ILDASERIALIZE_H_

#include "../../jansson/jansson.h"

int debug = 1;

ILDAHeader *serializeIldaHeader(json_t* ildaHeaderJson) {
	json_t* dirty = json_object_get(ildaHeaderJson, "dirty");
	json_t* maxDepth = json_object_get(ildaHeaderJson, "maxDepth");
	json_t* maxWidth = json_object_get(ildaHeaderJson, "maxWidth");
	json_t* maxHeight = json_object_get(ildaHeaderJson, "maxHeight");
	json_t* minDepth = json_object_get(ildaHeaderJson, "minDepth");
	json_t* minWidth = json_object_get(ildaHeaderJson, "minWidth");
	json_t* minHeight = json_object_get(ildaHeaderJson, "minHeight");
	json_t* nrOfFrames = json_object_get(ildaHeaderJson, "nrOfFrames");
	ILDAHeader *ildaHeader = malloc(sizeof(ILDAHeader));
	ildaHeader->dirty = json_number_value(dirty);
	ildaHeader->maxDepth = json_number_value(maxDepth);
	ildaHeader->maxWidth = json_number_value(maxWidth);
	ildaHeader->maxHeight = json_number_value(maxHeight);
	ildaHeader->minDepth = json_number_value(minDepth);
	ildaHeader->minWidth = json_number_value(minWidth);
	ildaHeader->minHeight = json_number_value(minHeight);
	ildaHeader->nrOfFrames = json_number_value(nrOfFrames);

	if(debug) {
		printf("ILDA Header:\n\tDirty: %i\n", ildaHeader->dirty);
		printf("\tMax Depth: %i\n", ildaHeader->maxDepth);
		printf("\tMax Width: %i\n", ildaHeader->maxWidth);
		printf("\tMax Height: %i\n", ildaHeader->maxHeight);
		printf("\tMin Depth: %i\n", ildaHeader->minDepth);
		printf("\tMin Width: %i\n", ildaHeader->minWidth);
		printf("\tMin Height: %i\n", ildaHeader->minHeight);
		printf("\tNr of frames: %i\n", ildaHeader->nrOfFrames);
	}
	return ildaHeader;
}

ColorData *serializeColorData(json_t* colorDataJson) {
	json_t* red1 = json_object_get(colorDataJson, "red1");
	json_t* red2 = json_object_get(colorDataJson, "red2");
	json_t* green1 = json_object_get(colorDataJson, "green1");
	json_t* green2 = json_object_get(colorDataJson, "green2");
	json_t* blue1 = json_object_get(colorDataJson, "blue1");
	json_t* blue2 = json_object_get(colorDataJson, "blue2");
	json_t* code = json_object_get(colorDataJson, "code");

	ColorData * colorData = malloc(sizeof(ColorData));
	colorData->red1 = json_number_value(red1);
	colorData->red2 = json_number_value(red2);
	colorData->green1 = json_number_value(green1);
	colorData->green2 = json_number_value(green2);
	colorData->blue1 = json_number_value(blue1);
	colorData->blue2 = json_number_value(blue2);
	colorData->code = json_number_value(code);
	if (debug) {
		printf("Color Data\n\tRed1: %i\n", colorData->red1);
		printf("\tRed2: %i\n", colorData->red2);
		printf("\tGreen1: %i\n", colorData->green1);
		printf("\tGreen2: %i\n", colorData->green2);
		printf("\tBlue1: %i\n", colorData->blue1);
		printf("\tBlue2: %i\n", colorData->blue2);
		printf("\tCode: %i\n", colorData->code);
	}
	return colorData;
}

CoordinateData *serializeCoordinateData(json_t* coordinateDataJson) {
	json_t* x = json_object_get(coordinateDataJson, "x");
	json_t* y = json_object_get(coordinateDataJson, "y");
	json_t* z = json_object_get(coordinateDataJson, "z");
	json_t* blanked = json_object_get(coordinateDataJson, "blanked");
	json_t* endImageData = json_object_get(coordinateDataJson, "endImageData");

	json_t* colorDataJson = json_object_get(coordinateDataJson, "colorData");
	ColorData *colorData = serializeColorData(colorDataJson);

	CoordinateData *coordinateData = malloc(sizeof(CoordinateData));
	coordinateData->x = json_number_value(x);
	coordinateData->y = json_number_value(y);
	coordinateData->z = json_number_value(z);
	coordinateData->colorData = colorData;
	coordinateData->blanked = json_number_value(blanked);
	coordinateData->endImageData = json_number_value(endImageData);
	if (0 && debug) {
		printf("Coordinate data:\n\tx: %i\n", coordinateData->x);
		printf("\ty: %i\n", coordinateData->y);
		printf("\tz: %i\n", coordinateData->z);
		printf("\tblanked: %i\n", coordinateData->blanked);
		printf("\tend image data: %i\n", coordinateData->endImageData);
	}
	return coordinateData;
}

CoordinateHeader *serializeCoordinateHeader(json_t* coordinateHeaderJson) {
	json_t* protocol = json_object_get(coordinateHeaderJson, "protocol");
	json_t* threeD = json_object_get(coordinateHeaderJson, "threeD");
	json_t* frameName = json_object_get(coordinateHeaderJson, "frameName");
	json_t* companyName = json_object_get(coordinateHeaderJson, "companyName");
	json_t* totalPoints = json_object_get(coordinateHeaderJson, "totalPoints");
	json_t* frameNumber = json_object_get(coordinateHeaderJson, "frameNumber");
	json_t* scannerHead = json_object_get(coordinateHeaderJson, "scannerHead");
	json_t *coordinateDatasJson = json_object_get(coordinateHeaderJson,
			"coordinateDatas");

	int coordinateDatasSize = json_array_size(coordinateDatasJson);
	CoordinateData **coordinateDatas = malloc(coordinateDatasSize * sizeof(CoordinateData*));
	int i;
	for (i = 0; i < coordinateDatasSize; i++) {
		json_t* coordinateDataJson = json_array_get(coordinateDatasJson, i);
		CoordinateData *coordinateData = serializeCoordinateData(coordinateDataJson);
		coordinateDatas[i] = coordinateData;
	}
	CoordinateHeader *coordinateHeader = malloc(sizeof(CoordinateHeader));
	coordinateHeader->protocol = strdup(json_string_value(protocol));
	coordinateHeader->threeD = json_number_value(threeD);
	coordinateHeader->frameName = strdup(json_string_value(frameName));
	coordinateHeader->companyName = strdup(json_string_value(companyName));
	coordinateHeader->totalPoints = json_number_value(totalPoints);
	coordinateHeader->frameNumber = json_number_value(frameNumber);
	coordinateHeader->scannerHead = json_number_value(scannerHead);
	coordinateHeader->coordinateDatas = coordinateDatas;
	coordinateHeader->totalCoordinateDatas = coordinateDatasSize;
	if(debug) {
		printf("Coordinate Header:\n\tProtocol: %s\n", coordinateHeader->protocol);
		printf("\tThreeD: %i\n", coordinateHeader->threeD);
		printf("\tFrame name: %s\n", coordinateHeader->frameName);
		printf("\tCompany name: %s\n", coordinateHeader->companyName);
		printf("\tTotal points: %i\n", coordinateHeader->totalPoints);
		printf("\tFrame nr: %i\n", coordinateHeader->frameNumber);
		printf("\tScanner head: %i\n", coordinateHeader->scannerHead);
	}
	return coordinateHeader;
}

ColorHeader *serializeColorHeader(json_t* colorHeader) {
	json_t* scannerHead = json_object_get(colorHeader, "scannerHead");
	json_t* paletteNumber = json_object_get(colorHeader, "paletteNumber");
	json_t* totalColors = json_object_get(colorHeader, "totalColors");
	json_t* companyName = json_object_get(colorHeader, "companyName");
	json_t* paletteName = json_object_get(colorHeader, "paletteName");
	json_t* formatCode = json_object_get(colorHeader, "formatCode");
	json_t* protocol = json_object_get(colorHeader, "protocol");
	ColorHeader *header = malloc(sizeof(ColorHeader));
	header->protocol = strdup(json_string_value(protocol));
	header->formatCode = json_number_value(formatCode);
	header->paletteName = strdup(json_string_value(paletteName));
	header->companyName = strdup(json_string_value(companyName));
	header->totalColors = json_number_value(totalColors);
	header->paletteNumber = json_number_value(paletteNumber);
	header->scannerHead = json_number_value(scannerHead);
	if (debug) {
		printf("Color header:\n\tscanner head: %i\n", header->scannerHead);
		printf("\tPalette nr: %i\n", header->paletteNumber);
		printf("\tTotal colors: %i\n", header->totalColors);
		printf("\tCompany name: %s\n", header->companyName);
		printf("\tPalette name: %s\n", header->paletteName);
		printf("\tFormat code: %i\n", header->formatCode);
		printf("\tProtocol: %s\n", header->protocol);
	}
	return header;
}

ILDA serialize(char* smsg, int smsgl) {
	json_t* root;
	json_error_t error;

	root = json_loadb(smsg, smsgl, 0, &error);
	if (!root) {
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
	}
	if (!json_is_object(root)) {
		fprintf(stderr, "error: commit data is not an object\n");
		json_decref(root);
	}
	json_t *ildaHeaderJson = json_object_get(root, "header");
	ILDAHeader *ildaHeader = serializeIldaHeader(ildaHeaderJson);
	json_t *colorHeaderJson = json_object_get(root, "colorHeader");
	ColorHeader *colorHeader = serializeColorHeader(colorHeaderJson);

	json_t *colorData = json_object_get(root, "colorDataList");
	int colorDatasSize = json_array_size(colorData);
	ColorData **colorDatas = malloc(colorDatasSize * sizeof(ColorData*));
	int i;
	for (i = 0; i < colorDatasSize; i++) {
		json_t* colorDataJson = json_array_get(colorData, i);
		ColorData *colorData = serializeColorData(colorDataJson);
		colorDatas[i] = colorData;
	}

	json_t *coordinateHeadersJson = json_object_get(root, "coordinateHeaders");
	unsigned int coordinateHeadersSize = json_array_size(coordinateHeadersJson);
	CoordinateHeader **coordinateHeaders = malloc(coordinateHeadersSize * sizeof(CoordinateHeader*));
	for (i = 0; i < coordinateHeadersSize; i++) {
		json_t* coordinateHeaderJson = json_array_get(coordinateHeadersJson, i);
		CoordinateHeader *coordinateHeader = serializeCoordinateHeader(
				coordinateHeaderJson);
		coordinateHeaders[i] = coordinateHeader;
	}

	json_decref(root);
	ILDA ilda = { ildaHeader, colorHeader, colorDatas, colorDatasSize,
			coordinateHeaders, coordinateHeadersSize };
	return ilda;
}


void destroyIlda(ILDA ilda) {
	free(ilda.ildaHeader);
	free(ilda.colorHeader);
	for(int i = 0;i<ilda.totalColorDatas;i++) {
		free(ilda.colorDatas[i]);
	}
	free(ilda.colorDatas);
	for(int i = 0;i<ilda.totalCoordinateHeaders;i++) {
		free(ilda.coordinateHeaders[i]);
	}
	free(ilda.coordinateHeaders);
}
#endif /* INCLUDE_ILDASERIALIZE_H_ */
