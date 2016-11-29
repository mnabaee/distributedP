/*
 * DockerApi.cpp
 *
 *  Created on: Jul 5, 2016
 *      Author: mahdy
 */

#include "DockerApi.h"
#include <utils/IP.h>
#include <utils/Error.h>

DockerApi::DockerApi(){

}

DockerApi::DockerApi(string endpoint) {
	endpointAddress = endpoint;
}

DockerApi::~DockerApi() {
	// TODO Auto-generated destructor stub
}

int DockerApi::createContainer(ContainerSpecs specs, int& containerId_ ){

	int newId;
	do{
		newId = rand();
	}while( containers.find(newId) != containers.end() );

	containers[newId] = specs;

	containers[newId].status = CONTAINER_CREATING;
	containers[newId].address = IP::GetOwnIP();

	string url = endpointAddress;
	url += "/containers/create";

	JsonObject jsonSpecs;
	jsonSpecs.PutInt("Memory", (int) specs.memory);
	jsonSpecs.PutString("Image", specs.image);
	jsonSpecs.PutString("Hostname", specs.hostname);
	for(unsigned int i = 0; i < specs.Cmd.size(); i++){
		jsonSpecs.PutStringInArray("Cmd", specs.Cmd[i]);
	}

	JsonObject exposed;
	JsonObject emptyJson;
	JsonObject bindings;
	for(unsigned int i = 0; i < specs.ports.size(); i++){
		exposed.PutJson( specs.ports[i].first + "/tcp" , emptyJson);
		JsonObject hostPort;
		hostPort.PutString("HostPort", specs.ports[i].second );
		bindings.PutJsonInArray( specs.ports[i].first + "/tcp", hostPort);
		jsonSpecs.PutJson("ExposedPorts", exposed);
	}
	jsonSpecs.PutJson("PortBindings", bindings);

	string result = httpClient.postRequest(url.c_str(), jsonSpecs.ToString().c_str(), HTTPClient::METHOD_POST, HTTPClient::FORMAT_JSON);

	JsonObject jsonMsg;
	if(JsonObject::ParseFromString(result, jsonMsg) &&
		jsonMsg.hasValue("Id")	){
		containers[newId].id = jsonMsg.GetString("Id");
		containers[newId].status = CONTAINER_CREATED;
		containerId_ = newId;
		return Error::NO_ERROR;
	}else{
		return Error::DOCKER_API_ERROR;
	}
}

JsonObject DockerApi::getContainerInfo(int containerId){
	JsonObject jsonMsg;
	if( containers.find(containerId) == containers.end() ){
		LogError("Could not find containerId = %d", containerId);
		return jsonMsg;
	}

	string url = endpointAddress;
	url += "/containers/";
	url += containers[containerId].id;
	url += "/json";

	string result = httpClient.postRequest(url.c_str(), "{}", HTTPClient::METHOD_GET, HTTPClient::FORMAT_JSON);

	JsonObject::ParseFromString(result, jsonMsg);
    return jsonMsg;
}

JsonObject DockerApi::getContainerTop(int containerId){
	JsonObject jsonMsg;
	if( containers.find(containerId) == containers.end() ){
		LogError("Could not find containerId = %d", containerId);
		return jsonMsg;
	}

	string url = endpointAddress;
	url += "/containers/";
	url += containers[containerId].id;
	url += "/top";

	string result = httpClient.postRequest(url.c_str(), "{}", HTTPClient::METHOD_GET, HTTPClient::FORMAT_JSON);

	JsonObject::ParseFromString(result, jsonMsg);
    return jsonMsg;
}

JsonObject DockerApi::getContainerLogs(int containerId){
	JsonObject jsonMsg;
	if( containers.find(containerId) == containers.end() ){
		LogError("Could not find containerId = %d", containerId);
		return jsonMsg;
	}

	string url = endpointAddress;
	url += "/containers/";
	url += containers[containerId].id;
	url += "/logs?stderr=1&stdout=1&timestamps=1";

	string result = httpClient.postRequest(url.c_str(), "{}", HTTPClient::METHOD_GET, HTTPClient::FORMAT_JSON);

	JsonObject::ParseFromString(result, jsonMsg);
    return jsonMsg;
}

int DockerApi::runContainer(int containerId){
	if( containers.find(containerId) == containers.end() ){
		LogError("Could not find containerId = %d", containerId);
		return Error::DOCKER_API_ERROR;
	}

	string url = endpointAddress;
	url += "/containers/";
	url += containers[containerId].id;
	url += "/start";

	string result = httpClient.postRequest(url.c_str(), "{}", HTTPClient::METHOD_POST, HTTPClient::FORMAT_JSON);
	return Error::NO_ERROR;
}

int DockerApi::stopContainer(int containerId){
	if( containers.find(containerId) == containers.end() ){
		LogError("Could not find containerId = %d", containerId);
		return Error::DOCKER_API_ERROR;
	}

	string url = endpointAddress;
	url += "/containers/";
	url += containers[containerId].id;
	url += "/stop";

	string result = httpClient.postRequest(url.c_str(), "{}", HTTPClient::METHOD_POST, HTTPClient::FORMAT_JSON);
	return Error::NO_ERROR;
}

int DockerApi::removeContainer(int containerId){
	if( containers.find(containerId) == containers.end() ){
		LogError("Could not find containerId = %d", containerId);
		return Error::DOCKER_API_ERROR;
	}

	string url = endpointAddress;
	url += "/containers/";
	url += containers[containerId].id;
	url += "";

	string result = httpClient.postRequest(url.c_str(), "{}", HTTPClient::METHOD_DELETE, HTTPClient::FORMAT_JSON);
	return Error::NO_ERROR;
}

JsonObject DockerApi::getImagesList(){

	string url = endpointAddress;
	url += "/images/json";
	string result = httpClient.postRequest(url.c_str(), "", HTTPClient::METHOD_GET);

	JsonObject jsonMsg;
	JsonObject::ParseFromString(result, jsonMsg);
    return jsonMsg;
}

JsonObject DockerApi::getContianersList(){
	string url = endpointAddress;
	url += "/containers/json?all=1";
	string result = httpClient.postRequest(url.c_str(), "", HTTPClient::METHOD_GET);

	JsonObject jsonMsg;
	JsonObject::ParseFromString(result, jsonMsg);
    return jsonMsg;
}

JsonObject DockerApi::pullImage(string imageName){

	string url = endpointAddress;
	url += "/images/create";
	string reqData="";
	reqData += imageName;
	string result = httpClient.postRequest(url.c_str(), reqData.c_str(), HTTPClient::METHOD_POST);

	JsonObject jsonMsg;
	JsonObject::ParseFromString(result, jsonMsg);
    return jsonMsg;
}



