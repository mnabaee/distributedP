/*
 * HTTPClient.h
 *
 *  Created on: Jan 17, 2016
 *      Author: mahdy
 */

#ifndef SRC_COMM_HTTPCLIENT_H_
#define SRC_COMM_HTTPCLIENT_H_

#include <curl/curl.h>
#include <utils/Logging.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <types/JsonObject.h>
#include <utils/DateTime.h>

class HTTPClient{
private:
	string readBuffer;

	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

public:

	static const char* METHOD_POST;
	static const char* METHOD_GET;
	static const char* METHOD_DELETE;

	static const char* FORMAT_URL_ENCODED;
	static const char* FORMAT_JSON;

	HTTPClient();
	~HTTPClient();

	string postRequest(const char* url, const char* requestData, const char* method = METHOD_POST, unsigned long timeout = 10, const char* format = FORMAT_URL_ENCODED);
	string postRequest(const char* url, JsonObject& jsonObj, const char* method = METHOD_POST,unsigned long timeout = 10 , const char* format = FORMAT_URL_ENCODED);

};


#endif /* SRC_COMM_HTTPCLIENT_H_ */
