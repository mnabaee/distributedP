/*
 * Bash.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: mahdy
 */

#include "Bash.h"
#include <memory>
#include <stdexcept>
#include <stdio.h>

Bash* Bash::mSingleton = NULL;

Bash::Bash(){

}

Bash::~Bash(){

}

Bash* Bash::GetInstance(){
	if(mSingleton == NULL){
		mSingleton = new Bash();
	}
	return mSingleton;
}

int Bash::executeBlockingSystemCommand(std::string command){
	return system(command.c_str());
}

int Bash::executeBlockingSystemCommand(const char* command){
	LogInfo("Executing %s ...", command);
	return system(command);
}

string Bash::executeBlockingSystemCommandWithResponse(const string& cmd){
    char buffer[1024*100];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    LogInfo("Finished opening pipe for %s", cmd.c_str());
    if (!pipe){
    	LogError("No Pipe for %s", cmd.c_str());
    }
    int cnt = 0;
    while (!feof(pipe.get())) {
    	//LogInfo("here1 for %d", cnt);
        if (fgets(buffer, 1024*100, pipe.get()) != NULL){
        	result += buffer;
        	//LogInfo("here2 for %d", cnt);
        }

    	cnt++;
    }
    return result;

}
