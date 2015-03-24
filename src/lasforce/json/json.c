/*
 * json.c
 *
 *  Created on: Mar 21, 2015
 *      Author: gijs
 */

#include "../../../include/lasforce/json/json.h"
#include <string.h>
#include <syslog.h>

int getInt(json_t* root, char* key) {
	json_t *value = json_object_get(root, key);
	if (value == NULL) {
		syslog(LOG_DEBUG, "Can't find the key: %s in json: %s", key, json_dumps(root, 0));
		return 0;
	} else {
		int val = json_number_value(value);
//		syslog(LOG_DEBUG, "getInt: %i\n", val);
		json_decref(value);
		return val;
	}
}

char* getString(json_t* root, char* key) {
	json_t *value = json_object_get(root, key);
	if (value == NULL) {
		syslog(LOG_DEBUG, "Can't find the key: %s in json: %s", key, json_dumps(root, 0));
		return "";
	} else {
		char* val = strdup(json_string_value(value));
//		syslog(LOG_DEBUG, "getString: %s\n", val);
		json_decref(value);
		return val;
	}
}



