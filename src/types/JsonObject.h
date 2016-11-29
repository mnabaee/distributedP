/*
 * JsonObject.h
 *
 *  Created on: Jan 20, 2016
 *      Author: mahdy
 */

#ifndef SRC_TYPES_JSONOBJECT_H_
#define SRC_TYPES_JSONOBJECT_H_

#include "jsoncpp/json/json.h"
#include <string>
#include "utils/Logging.h"

class JsonObject{
private:
	Json::Value jsonValue;
public:

	JsonObject();
	JsonObject(JsonObject& jo);
	JsonObject(const JsonObject& jo);
	void copyTo(JsonObject& jo);

	~JsonObject();
	std::string GetString(const std::string & key);
	int GetInt(const std::string & key);
	unsigned int GetUInt(const std::string & key);
	bool GetBoolean(const std::string & key);
	JsonObject GetJson(const std::string & key);
	double GetDouble(const std::string & key);


	void PutString(const std::string & key, const std::string & val);
	void PutInt(const std::string & key, const int & val);
	void PutUInt(const std::string & key, const unsigned int & val);
	void PutBoolean(const std::string & key, const bool & val);
	void PutJson(const std::string & key, const JsonObject & val);

	void PutArray(const std::string& key);
	void PutUIntInArray(const std::string & key, const unsigned int val);
	void PutStringInArray(const std::string & key, const std::string & val);
	void PutJsonInArray(const std::string & key, const JsonObject & val);

	unsigned int GetArraySize(const std::string& key);
	JsonObject GetJsonFromArray(const std::string& key, unsigned int index);
	string GetStringFromArray(const std::string& key, unsigned int index);
	unsigned int GetUIntFromArray(const std::string& key, unsigned int index);

	bool hasValue(const std::string & key);

	std::string ToString();
	std::string ToStringOneLine();

	static bool ParseFromString(const std::string & string, JsonObject & jsonObj);


};



#endif /* SRC_TYPES_JSONOBJECT_H_ */
