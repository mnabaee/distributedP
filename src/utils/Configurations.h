/*
 * Configurations.h
 *
 *  Created on: Jul 31, 2016
 *      Author: mahdy
 */

#ifndef UTILS_CONFIGURATIONS_H_
#define UTILS_CONFIGURATIONS_H_

#include <types/JsonObject.h>

class Configurations {
	JsonObject* config;
public:
	Configurations();
	virtual ~Configurations();

	bool parseFromString(const string& str);
	bool parseFromFile(const string& filename);
	void addConfig(const string& sectionName, const JsonObject& config);
	JsonObject* GetJson(){return config;};
};

#endif /* UTILS_CONFIGURATIONS_H_ */
