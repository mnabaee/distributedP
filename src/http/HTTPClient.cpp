/*
 * HTTPClient.cpp
 *
 *  Created on: Jan 17, 2016
 *      Author: mahdy
 */

#include "HTTPClient.h"
#include "HTTPServer.h"

const char* HTTPClient::METHOD_POST = "POST";
const char* HTTPClient::METHOD_GET = "GET";
const char* HTTPClient::METHOD_DELETE = "DELETE";

const char* HTTPClient::FORMAT_URL_ENCODED = "Content-Type: application/x-www-form-urlencoded";
const char* HTTPClient::FORMAT_JSON = "Content-Type: application/json";

HTTPClient::HTTPClient(){
	curl_global_init(CURL_GLOBAL_ALL);
}

HTTPClient::~HTTPClient(){

}

string HTTPClient::postRequest(const char* url, JsonObject & jsonObj, const char* method, unsigned long timeout, const char* format){

	JsonObject newJO(jsonObj);
	newJO.PutUInt("tx_time", current_date_time_msec());
	srand(time(NULL));
	unsigned int uniqueId = (unsigned int) rand();
	newJO.PutUInt("unique_message_id", uniqueId);
	string contentStr = string(POST_DATA_FIELD) + "=" + newJO.ToStringOneLine();
	return postRequest(url, contentStr.c_str(), method, timeout, HTTPClient::FORMAT_URL_ENCODED);
}


string HTTPClient::postRequest(const char* url, const char* requestData, const char* method, unsigned long timeout, const char* format){

	string resBuffer;
	/* get a curl handle */

	  CURL* curl = curl_easy_init();
	  //CURLcode res;

      int curlRc = CURLE_OK;
      int reqDataSize = 0;

      if(requestData != NULL){
    	  reqDataSize = strlen(requestData);
      }

      //const char* method = "POST";

      curlRc = curl_easy_setopt(curl, CURLOPT_URL, url);
      if (curlRc == CURLE_OK) {
    	  curlRc = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    	  curlRc = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resBuffer);
          curlRc = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, reqDataSize);
          curlRc = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
          if (curlRc == CURLE_OK) {
          	curlRc = -100;
  				curlRc = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestData);
  				if (curlRc == CURLE_OK) {
  					if (method != NULL) {
  						// In case the required method is not POST.
  						curlRc = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
  					}
  					if (curlRc == CURLE_OK) {
  						// Set the various headers
  						struct curl_slist *headers = NULL;
  						headers = curl_slist_append(headers,
  								format);
  						curlRc = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  						if (curlRc == CURLE_OK) {
  										curlRc = curl_easy_perform(curl);
  						}
  						if (headers != NULL) {
  							curl_slist_free_all(headers);
  						}
  					}
  				}

          }
          if( curlRc == CURLE_OPERATION_TIMEDOUT  ){
        	  LogWarning("Curl operation timeout (%lu) for url = %s", timeout, url);
        	  resBuffer = "";
          }
          curlRc = curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
          curlRc = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
      }
      curl_easy_cleanup(curl);
	  return resBuffer;
}

size_t HTTPClient::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
