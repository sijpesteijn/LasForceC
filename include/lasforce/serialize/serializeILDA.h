/*
 * serializeILDA.h
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */

#ifndef INCLUDE_LASFORCE_SERIALIZE_SERIALIZEILDA_H_
#define INCLUDE_LASFORCE_SERIALIZE_SERIALIZEILDA_H_

#include <errno.h>
#include "../ilda/ILDA.h"

ILDAHeader *serializeIldaHeader(json_t* ildaHeaderJson);

ColorData *serializeColorData(json_t* colorDataJson);

CoordinateData *serializeCoordinateData(json_t* coordinateDataJson);

CoordinateHeader *serializeCoordinateHeader(json_t* coordinateHeaderJson);

ColorHeader *serializeColorHeader(json_t* colorHeader);

ILDA* serializeILDA(json_t* root);

#endif /* INCLUDE_LASFORCE_SERIALIZE_SERIALIZEILDA_H_ */

