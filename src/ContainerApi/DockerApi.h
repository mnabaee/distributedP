/*
 * DockerApi.h
 *
 *  Created on: Jul 5, 2016
 *      Author: mahdy
 */

#ifndef TARGETS_MANAGER_DOCKERAPI_H_
#define TARGETS_MANAGER_DOCKERAPI_H_

#include "ContainerApi.h"
#include <http/HTTPClient.h>

class DockerApi: public ContainerApi {
	HTTPClient httpClient;
	string endpointAddress;

public:
	DockerApi();
	DockerApi(string endpoint);
	virtual ~DockerApi();

	JsonObject getImagesList();
	JsonObject pullImage(string imageName);
	JsonObject getContianersList();
	JsonObject getContainerInfo(int containerId);
	JsonObject getContainerTop(int containerId);
	JsonObject getContainerLogs(int containerId);

	int createContainer(ContainerSpecs specs, int& id );
	int runContainer(int containerId);
	int stopContainer(int containerId);
	int removeContainer(int containerId);

};

#endif /* TARGETS_MANAGER_DOCKERAPI_H_ */
