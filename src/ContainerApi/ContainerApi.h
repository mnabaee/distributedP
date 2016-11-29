/*
 * ContainerApi.h
 *
 *  Created on: Jul 5, 2016
 *      Author: mahdy
 */

#ifndef TARGETS_MANAGER_CONTAINERAPI_H_
#define TARGETS_MANAGER_CONTAINERAPI_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

class ContainerApi {

public:

	enum ContainerStatus{
		CONTAINER_CREATING,
		CONTAINER_CREATED,
		CONTAINER_RUNNING,
		CONTAINER_STOPPED,
		CONTAINER_TERMINATED
	};

	struct ContainerSpecs{
		unsigned int memory;
		string image;
		bool forcePorts;
		string hostname;
		vector<string> Cmd;
		vector<pair<string,string> > ports;  //containerPort - hostPort

		string id;
		string address;
		int status;
		ContainerSpecs(){
			memory = 64000000;
			image = "mnabaee/crawler";
			ports.clear();
			ports.push_back(pair<string,string>("8000", "8000") );
			ports.push_back(pair<string,string>("22", "8086") );
			forcePorts = false;
			hostname = "";
			address = "";
			status = CONTAINER_CREATING;

			Cmd.clear();
			Cmd.push_back("/bin/sh");
			Cmd.push_back("-c");
			Cmd.push_back("  while :; do echo 'Hit CTRL+C'; sleep 1; done ");

		}

		void downloadAndRun(string address){
			Cmd.clear();
			Cmd.push_back("/bin/sh");
			Cmd.push_back("-c");
			string oneLineCmd = " wget -O /run.sh " + address + " ; ";
			oneLineCmd += " chmod +x /run.sh ; ./run.sh ";
			Cmd.push_back(oneLineCmd);

		}

	};

	ContainerApi();
	virtual ~ContainerApi();

	ContainerSpecs* GetContainerSpecs(int containerId);

	virtual int createContainer(ContainerSpecs specs, int& id ) = 0;
	virtual int runContainer(int containerId) = 0;
	virtual int stopContainer(int containerId) = 0;
	virtual int removeContainer(int containerId) = 0;

protected:
	map<int, ContainerSpecs> containers;

};

#endif /* TARGETS_MANAGER_CONTAINERAPI_H_ */
