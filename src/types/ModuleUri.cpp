/*
 * UnitRef.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: mahdy
 */

#include "ModuleUri.h"

ModuleUri::ModuleUri(){
	hostName = INVALID_HOST_NAME;
	httpPort = Error::INVALID_PORT_NUM;
	tcpPort = Error::INVALID_PORT_NUM;
	exeName = INVALID_EXE_NAME;
	processId = INVALID_PROC_ID;
	arrangeFullStr();
}

ModuleUri::ModuleUri(const std::string & hostName, int tcpPort, int httpPort, std::string exeName, int processId){
	this->hostName = hostName;
	this->tcpPort = tcpPort;
	this->httpPort = httpPort;
	this->exeName = exeName;
	this->processId = processId;
	arrangeFullStr();
}



ModuleUri::~ModuleUri(){

}

std::string ModuleUri::GetFullString(){
	return fullString;
}

void ModuleUri::SetHostName(const std::string & hostName){
	this->hostName = hostName;
	arrangeFullStr();
}

void ModuleUri::SetHttpPort(int httpPort){
	this->httpPort = httpPort;
	arrangeFullStr();
}

void ModuleUri::SetTcpPort(int tcpPort){
	this->tcpPort = tcpPort;
	arrangeFullStr();
}

void ModuleUri::SetProcessId(int pid){
	this->processId = pid;
}

void ModuleUri::SetExeName(std::string exeName){
	this->exeName = exeName;
}

void ModuleUri::SetExeName(const char* exeName){
	this->exeName.assign(exeName);
}

std::string ModuleUri::GetHostName(){
	return hostName;
}

int ModuleUri::GetTcpPort(){
	return tcpPort;
}

int ModuleUri::GetHttpPort(){
	return httpPort;
}

void ModuleUri::arrangeFullStr(){
	char fullRef[1024];
	sprintf (fullRef,"%s:%d:%d|%s|%d", this->hostName.c_str(), this->httpPort, this->tcpPort, this->exeName.c_str(), this->processId);
	this->fullString.assign(fullRef);
}

bool ModuleUri::isTheSame(ModuleUri& uri){
	return ( uri.GetHostName() == this->GetHostName() &&
			 uri.GetTcpPort() == this->GetTcpPort());
}

bool ModuleUri::isTheSame(const ModuleUri& uri){
	ModuleUri uriCpy = uri;
	return ( uriCpy.GetHostName() == this->GetHostName() &&
			uriCpy.GetTcpPort() == this->GetTcpPort());
}

bool ModuleUri::onTheSameHost(ModuleUri& uri){
	return (uri.hostName == this->hostName);
}

std::string ModuleUri::GetOwnExeName(){

	std::ifstream comm("/proc/self/comm");
	std::string name;
	getline(comm, name);
	std::string res(name);
	return res;
//	    std::cout << "my name is " << name << '\n';

}

void ModuleUri::SetOwnUri(){
	this->SetExeName( GetOwnExeName() );
	this->SetHostName( IP::GetOwnIP() );
	this->SetProcessId( getpid() );
}

bool ModuleUri::assign(const string& inStr){
	ModuleUri uri;
	string str = inStr;
	int i = str.find(":");
	if( i < 0 ) return false;
	uri.SetHostName( str.substr(0, i) );
	str = str.substr(i + 1);

	i = str.find(":");
	if( i < 0 ) return false;
	uri.SetHttpPort( atoi( str.substr(0, i).c_str() ) );
	str = str.substr(i + 1);

	i = str.find("|");
	if( i < 0 ) return false;
	uri.SetTcpPort( atoi( str.substr(0, i).c_str() ) );
	str = str.substr(i + 1);

	i = str.find("|");
	if(i < 0 || i >= (int) str.length() ) return false;
	uri.SetExeName( str.substr(0, i-1) );
	uri.SetProcessId( atoi( str.substr(i+1).c_str() ) );

	this->copyFrom( uri );
	return true;
}

void ModuleUri::copyFrom(ModuleUri& uri){
	this->exeName = uri.exeName;
	this->tcpPort = uri.tcpPort;
	this->hostName = uri.hostName;
	this->httpPort = uri.httpPort;
	this->processId = uri.processId;
	this->arrangeFullStr();
}
