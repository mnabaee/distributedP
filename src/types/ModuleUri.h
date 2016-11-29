

#ifndef TYPES_MODULEURI_H_
#define TYPES_MODULEURI_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "utils/Error.h"
#include "utils/Logging.h"

#define INVALID_HOST_NAME "UNKNOWN"
#define INVALID_EXE_NAME ".out"
#define INVALID_PROC_ID -9999

class ModuleUri{
private:
	std::string hostName;
	int httpPort;
	int tcpPort;
	int processId;
	std::string exeName;

	std::string fullString;

	void arrangeFullStr();

public:
	ModuleUri();
	void copyFrom(ModuleUri& uri);
	ModuleUri(const std::string & hostName, int tcpPort, int httpPort, std::string exeName = INVALID_EXE_NAME, int processId = INVALID_PROC_ID);

	~ModuleUri();

	void SetHostName(const std::string & hostName);
	void SetHttpPort(int port);
	void SetTcpPort(int port);
	void SetProcessId(int pid);
	void SetExeName(std::string exeName);
	void SetExeName(const char* exeName);
	bool assign(const string& str);

	std::string GetHostName();
	int GetHttpPort();
	int GetTcpPort();
	std::string GetExeName(){return this->exeName;};
	int GetProcessId(){return this->processId;};

	std::string GetFullString();
	string GetUrl(){return hostName + ":" + to_string(httpPort);};

	static std::string GetOwnExeName();
	bool isTheSame(ModuleUri& uri);
	bool isTheSame(const ModuleUri& uri);
	bool onTheSameHost(ModuleUri& uri);

	bool isValid(){return (hostName != INVALID_HOST_NAME);};
	void SetOwnUri();

};


#endif /* TYPES_MODULEURI_H_ */
