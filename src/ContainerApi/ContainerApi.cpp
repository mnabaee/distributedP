/*
 * ContainerApi.cpp
 *
 *  Created on: Jul 5, 2016
 *      Author: mahdy
 */

#include "ContainerApi.h"
#include <utils/Logging.h>

ContainerApi::ContainerApi() {
	containers.clear();
}

ContainerApi::~ContainerApi() {
	// TODO Auto-generated destructor stub
}

ContainerApi::ContainerSpecs* ContainerApi::GetContainerSpecs(int containerId){
	if( containers.find(containerId) == containers.end() ){
		LogError("Could not find containerId = %d", containerId);
		return NULL;
	}

	return & containers[containerId];
}
