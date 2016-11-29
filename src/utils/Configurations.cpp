/*
 * Configurations.cpp
 *
 *  Created on: Jul 31, 2016
 *      Author: mahdy
 */

#include "Configurations.h"
#include <utils/Pagedownloader.h>

Configurations::Configurations() {
	config = new JsonObject;
}

Configurations::~Configurations() {
	delete config;
}

void Configurations::addConfig(const string& sectionName, const JsonObject& config_){
	config->PutJson(sectionName, config_);
}

bool Configurations::parseFromString(const string& str){
	return JsonObject::ParseFromString(str, *config);
}

bool Configurations::parseFromFile(const string& filename){
	LogInfo("Parsing configuration from filename=%s", filename.c_str());
	string content;
	if( !Pagedownloader::readTextFile(filename, content)){
		LogError("Failed to open configuration filename=%s", filename.c_str() );
		return false;
	}
	return parseFromString(content);
}
