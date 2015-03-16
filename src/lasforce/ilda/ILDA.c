/*
 * ILDA.c
 *
 *  Created on: Mar 16, 2015
 *      Author: gijs
 */

#include "../../../include/lasforce/ilda/ILDA.h"
#include "../../../include/lf.h"

void destroyIlda(ILDA ilda) {
	if (ilda.ildaHeader != NULL)
		free(ilda.ildaHeader);
	if (ilda.colorHeader != NULL)
		free(ilda.colorHeader);
	int i;
	if (ilda.totalColorDatas > 0) {
		for(i = 0;i<ilda.totalColorDatas;i++) {
			free(ilda.colorDatas[i]);
		}
		free(ilda.colorDatas);
	}
	if (ilda.totalCoordinateHeaders > 0) {
		for(i = 0;i<ilda.totalCoordinateHeaders;i++) {
			free(ilda.coordinateHeaders[i]);
		}
		free(ilda.coordinateHeaders);
	}
}


