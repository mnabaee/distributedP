/*
 * JsonObject.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: mahdy
 */

#include "JsonObject.h"
#include <vector>

JsonObject::JsonObject(){
	Json::Reader reader;
	reader.parse("{}", this->jsonValue);
}

JsonObject::JsonObject(JsonObject& jo){
	this->jsonValue = jo.jsonValue;
}

JsonObject::JsonObject(const JsonObject& jo){
	this->jsonValue = jo.jsonValue;
}

void JsonObject::copyTo(JsonObject& jo){
	jo.jsonValue = this->jsonValue;
}

JsonObject::~JsonObject(){
}

string JsonObject::ToString(){
	return jsonValue.toStyledString();
}

string JsonObject::ToStringOneLine(){
	   Json::FastWriter writer;
	   return writer.write( jsonValue );
}

void JsonObject::PutString(const string & key, const string & val){
	jsonValue[key] = val;
}

void JsonObject::PutInt(const string & key, const int & val){
	jsonValue[key] = val;
}

void JsonObject::PutUInt(const string & key, const unsigned int & val){
	jsonValue[key] = val;
}

void JsonObject::PutBoolean(const string & key, const bool & val){
	jsonValue[key] = val;
}

void JsonObject::PutJson(const std::string & key, const JsonObject & val){
	jsonValue[key] = val.jsonValue;
}

void JsonObject::PutArray(const std::string& key){
	if( hasValue(key) && !jsonValue[key].isArray() ){
		LogError("%s is not an array!", key.c_str());
	}else if(!hasValue(key)){
		Json::Value array(Json::arrayValue);
		jsonValue[key] = array;
	}
}

void JsonObject::PutUIntInArray(const std::string & key, const unsigned int val){
	if( hasValue(key) ){
		if( jsonValue[key].isArray() ){
			jsonValue[key].append(val);
		}else{
			LogError("%s is not an array!", key.c_str());
		}
	}else{
		Json::Value array;
		array.append(val);
		jsonValue[key] = array;
	}
}

void JsonObject::PutStringInArray(const std::string & key, const std::string & val){
	if( hasValue(key) ){
		if( jsonValue[key].isArray() ){
			jsonValue[key].append(val);
		}else{
			LogError("%s is not an array!", key.c_str());
		}
	}else{
		Json::Value array;
		array.append(val);
		jsonValue[key] = array;
	}
}

void JsonObject::PutJsonInArray(const std::string & key, const JsonObject & val){
	if( hasValue(key) ){
		if( jsonValue[key].isArray() ){
			jsonValue[key].append( val.jsonValue );
		}else{
			LogError("%s is not an array!", key.c_str());
		}
	}else{
		Json::Value array;
		array.append( val.jsonValue );
		jsonValue[key] = array;
	}
}

string JsonObject::GetString(const string & key){
	return jsonValue[key].asString();
}

int JsonObject::GetInt(const string & key){
	return jsonValue[key].asInt();
}

unsigned int JsonObject::GetUInt(const string & key){
	return jsonValue[key].asUInt();
}

bool JsonObject::GetBoolean(const string & key){
	return jsonValue[key].asBool();
}

double JsonObject::GetDouble(const std::string & key){
	return jsonValue[key].asDouble();
}

JsonObject JsonObject::GetJson(const std::string & key){
	JsonObject res;
	res.jsonValue = jsonValue[key];
	return res;
}

unsigned int JsonObject::GetArraySize(const std::string& key){
	if( hasValue(key) ){
		return jsonValue[key].size();
	}else{
		LogError("Missing key = %s", key.c_str());
		return 0;
	}
}

unsigned int JsonObject::GetUIntFromArray(const std::string& key, unsigned int index){
	unsigned int res;
	if( GetArraySize(key) > 0 ){
		res = jsonValue[key][index].asUInt();
	}else{
		LogError("Array %s has a size of zero!");
	}
	return res;
}

string JsonObject::GetStringFromArray(const std::string& key, unsigned int index){
	string res;
	if( GetArraySize(key) > 0 ){
		res = jsonValue[key][index].asString();
	}else{
		LogError("Array %s has a size of zero!");
	}
	return res;
}

JsonObject JsonObject::GetJsonFromArray(const std::string& key, unsigned int index){
	JsonObject res;
	if( GetArraySize(key) > 0 ){
		res.jsonValue = jsonValue[key][index];
	}else{
		LogError("Array %s has a size of zero!");
	}
	return res;
}

bool JsonObject::hasValue(const string & key){
	return jsonValue.isMember(key);
}

bool JsonObject::ParseFromString(const string & string, JsonObject & jsonObj){
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( string, jsonObj.jsonValue );

	if ( !parsingSuccessful )
	{
		LogError("Failed to parse string to json:\n%s", string.c_str() );
	    return false;
	}else{
		return true;
	}
}
