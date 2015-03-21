/*
 * serializeILDA.c
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */

#include "../../../include/lf.h"
#include "../../../include/lasforce/serialize/serializeCommands.h"
#include "../../../include/lasforce/ilda/ILDA.h"
#include "../../../include/lasforce/json/json.h"
#include <errno.h>

ILDAHeader *serializeIldaHeader(json_t* ildaHeaderJson) {
	ILDAHeader *ildaHeader = malloc(sizeof(ILDAHeader));
	if (ildaHeader == NULL) {
		perror("Can't allocate memory for IldaHeader.");
		strerror(errno);
		exit(1);
	}
	ildaHeader->dirty = getInt(ildaHeaderJson, "dirty");
	ildaHeader->maxDepth = getInt(ildaHeaderJson, "maxDepth");
	ildaHeader->maxWidth = getInt(ildaHeaderJson, "maxWidth");
	ildaHeader->maxHeight = getInt(ildaHeaderJson, "maxHeight");
	ildaHeader->minDepth = getInt(ildaHeaderJson, "minDepth");
	ildaHeader->minWidth = getInt(ildaHeaderJson, "minWidth");
	ildaHeader->minHeight = getInt(ildaHeaderJson, "minHeight");
	ildaHeader->nrOfFrames = getInt(ildaHeaderJson, "nrOfFrames");

	return ildaHeader;
}

ColorData *serializeColorData(json_t* colorDataJson) {

	ColorData * colorData = malloc(sizeof(ColorData));
	if (colorData == NULL) {
		perror("Can't allocate memory for ColorData.");
		strerror(errno);
		exit(1);
	}
	colorData->red1 = getInt(colorDataJson, "red1");
	colorData->red2 = getInt(colorDataJson, "red2");
	colorData->green1 = getInt(colorDataJson, "green1");
	colorData->green2 = getInt(colorDataJson, "green2");
	colorData->blue1 = getInt(colorDataJson, "blue1");
	colorData->blue2 = getInt(colorDataJson, "blue2");
	colorData->code = getInt(colorDataJson, "code");

	return colorData;
}

CoordinateData *serializeCoordinateData(json_t* coordinateDataJson) {
	CoordinateData *coordinateData = malloc(sizeof(CoordinateData));
	if (coordinateData == NULL) {
		perror("Can't allocate memory for CoordinateData.");
		strerror(errno);
		exit(1);
	}

	json_t* colorDataJson = json_object_get(coordinateDataJson, "colorData");
	ColorData *colorData = serializeColorData(colorDataJson);

	coordinateData->x = getInt(colorDataJson, "x");
	coordinateData->y = getInt(colorDataJson, "y");
	coordinateData->z = getInt(colorDataJson, "z");
	coordinateData->colorData = colorData;
	coordinateData->blanked = getInt(colorDataJson, "blanked");
	coordinateData->endImageData = getInt(colorDataJson, "endImageData");

	return coordinateData;
}

CoordinateHeader *serializeCoordinateHeader(json_t* coordinateHeaderJson) {
	json_t *coordinateDatasJson = json_object_get(coordinateHeaderJson, "coordinateDatas");
	int coordinateDatasSize = json_array_size(coordinateDatasJson);
	CoordinateData **coordinateDatas = malloc(coordinateDatasSize * sizeof(CoordinateData*));
	if (coordinateDatas == NULL) {
		perror("Can't allocate memory for CoordinateData pointer array.");
		strerror(errno);
		exit(1);
	}

	int i;
	for (i = 0; i < coordinateDatasSize; i++) {
		json_t* coordinateDataJson = json_array_get(coordinateDatasJson, i);
		CoordinateData *coordinateData = serializeCoordinateData(coordinateDataJson);
		coordinateDatas[i] = coordinateData;
	}
	CoordinateHeader *coordinateHeader = malloc(sizeof(CoordinateHeader));
	if (coordinateHeader == NULL) {
		perror("Can't allocate memory for CoordinateHeader.");
		strerror(errno);
		exit(1);
	}
	coordinateHeader->protocol = getString(coordinateHeaderJson, "protocol");
	coordinateHeader->threeD = getInt(coordinateHeaderJson, "threeD");
	coordinateHeader->frameName = getString(coordinateHeaderJson, "frameName");
	coordinateHeader->companyName = getString(coordinateHeaderJson, "companyName");
	coordinateHeader->totalPoints = getInt(coordinateHeaderJson, "totalPoints");
	coordinateHeader->frameNumber = getInt(coordinateHeaderJson, "frameNumber");
	coordinateHeader->scannerHead = getInt(coordinateHeaderJson, "scannerHead");
	coordinateHeader->coordinateDatas = coordinateDatas;
	coordinateHeader->totalCoordinateDatas = coordinateDatasSize;

	return coordinateHeader;
}

ColorHeader *serializeColorHeader(json_t* colorHeader) {
	ColorHeader *header = malloc(sizeof(ColorHeader));
	if (header == NULL) {
		perror("Can't allocate memory for ColorHeader.");
		strerror(errno);
		exit(1);
	}

	header->protocol = getString(colorHeader, "protocol");
	header->formatCode = getInt(colorHeader, "formatCode");
	header->paletteName = getString(colorHeader, "paletteName");
	header->companyName = getString(colorHeader, "companyName");
	header->totalColors = getInt(colorHeader, "totalColors");
	header->paletteNumber = getInt(colorHeader, "paletteNumber");
	header->scannerHead = getInt(colorHeader, "scannerHead");

	return header;
}

ILDA* serializeILDA(json_t* root) {
	ILDAHeader *ildaHeader = NULL;
	ColorHeader *colorHeader = NULL;
	ColorData **colorDatas = NULL;
	int colorDatasSize = 0;
	CoordinateHeader **coordinateHeaders = NULL;
	int coordinateHeadersSize = 0;
	json_t *idJson = json_object_get(root,"id");
	int id = json_number_value(idJson);
//	printf("ILDA ID: %i\n", id);
	json_t *ildaHeaderJson = json_object_get(root, "header");
	if (ildaHeaderJson != NULL) {
		ildaHeader = serializeIldaHeader(ildaHeaderJson);
	}
	json_t *colorHeaderJson = json_object_get(root, "colorHeader");
	if (colorHeaderJson != NULL) {
		colorHeader = serializeColorHeader(colorHeaderJson);
	}

	json_t *colorData = json_object_get(root, "colorDataList");
	if (colorData != NULL) {
		colorDatasSize = json_array_size(colorData);
		colorDatas = malloc(colorDatasSize * sizeof(ColorData*));
		int i;
		for (i = 0; i < colorDatasSize; i++) {
			json_t* colorDataJson = json_array_get(colorData, i);
			ColorData *colorData = serializeColorData(colorDataJson);
			colorDatas[i] = colorData;
		}
	}

	json_t *coordinateHeadersJson = json_object_get(root, "coordinateHeaders");
	if (coordinateHeadersJson != NULL) {
		coordinateHeadersSize = json_array_size(coordinateHeadersJson);
		coordinateHeaders = malloc(coordinateHeadersSize * sizeof(CoordinateHeader*));
		int i;
		for (i = 0; i < coordinateHeadersSize; i++) {
			json_t* coordinateHeaderJson = json_array_get(coordinateHeadersJson, i);
			CoordinateHeader *coordinateHeader = serializeCoordinateHeader(
					coordinateHeaderJson);
			coordinateHeaders[i] = coordinateHeader;
		}
	}

	ILDA* ilda = malloc(sizeof(ILDA));
	ilda->id = id;
	ilda->ildaHeader = ildaHeader;
	ilda->colorHeader = colorHeader;
	ilda->colorDatas = colorDatas;
	ilda->totalColorDatas = colorDatasSize;
	ilda->coordinateHeaders = coordinateHeaders;
	ilda->totalCoordinateHeaders = coordinateHeadersSize;

	return ilda;
}

