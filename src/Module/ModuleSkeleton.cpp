/*
 * ModuleSkeleton.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: mahdy
 */

#include "ModuleSkeleton.h"
#include <libgen.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <utils/Bash.h>
#include <utils/Pagedownloader.h>

const int ModuleSkeleton::UNDEFINED_SERVER_PORT = -1;
const string ModuleSkeleton::JSON_HANDLING_ERROR = "ERROR HANDLING JSON";
const string ModuleSkeleton::NOT_IMPLEMENTED = "HANDLER NOT IMPLEMENTED";
const string ModuleSkeleton::SERVER_ERROR = "INTERNAL SERVER ERROR";
ModuleSkeleton* ModuleSkeleton::singleton = NULL;

ModuleSkeleton::ModuleSkeleton() {
	isMainModule = false;
	httpClient = new HTTPClient;
	httpServer = NULL;
	fileServer = NULL;
	fileServerRootPath = "";
	fileServerSubDomain = "";
	configFilename = "";
	spawnId = Error::INVALID_PARAM;

	jsonHandler = NULL;
	getHandler = NULL;
	postHandler = NULL;
	moduleConfig = NULL;
	logger = NULL;
	peers = NULL;
	interrupthandler = NULL;
}

ModuleSkeleton::~ModuleSkeleton() {
	delete httpClient;
}

void ModuleSkeleton::handleConfig(){
	if(moduleConfig->GetJson()->hasValue("TcpPort")){
		createTcpPeerManager( moduleConfig->GetJson()->GetInt("TcpPort") );
	}
	if(moduleConfig->GetJson()->hasValue("HttpPort")){
		createHttpServer( moduleConfig->GetJson()->GetInt("HttpPort") );
	}
	if(moduleConfig->GetJson()->hasValue("initialModuleUri")){
		initialModuleUri.assign( moduleConfig->GetJson()->GetString("initialModuleUri") );
	}
	if(moduleConfig->GetJson()->hasValue("spawnId")){
		spawnId = moduleConfig->GetJson()->GetInt("spawnId");
	}
	if(moduleConfig->GetJson()->hasValue("FileServerSubDomain") &&
			moduleConfig->GetJson()->hasValue("FileServerDirectory")){
		createFileServer(moduleConfig->GetJson()->GetString("FileServerDirectory"),
				         moduleConfig->GetJson()->GetString("FileServerSubDomain")	);
	}
	if( moduleConfig->GetJson()->hasValue("Logging") ){
		JsonObject loggingJson = moduleConfig->GetJson()->GetJson("Logging");
		if( loggingJson.hasValue("FileName") ){
			string dir_ = "~/";
			if(loggingJson.hasValue("Dir")){
				dir_ = loggingJson.GetString("Dir");
			}
			string logfilename = loggingJson.GetString("FileName");
			if(logfilename == ""){
				 srand (time(NULL));
				 logfilename = "log_" + ModuleUri::GetOwnExeName() + "_" + to_string(getpid()) + "_" + to_string(rand()) + ".log";
			}
			logfilename = dir_ + logfilename;
			logger->SetLogFileName(logfilename);
			LogInfo("Logging is initiated to save in file = %s", logfilename.c_str());
		}
	}
}

void ModuleSkeleton::initSkeleton(int argc, char** argv, bool isMainModule){

	if(singleton){
		LogWarning(" Module Skeleton singleton is already created!");
		return;
	}

	singleton = new ModuleSkeleton;
	singleton->logger = Logging::getInstance();
	singleton->isMainModule = isMainModule;
	singleton->moduleConfig = new Configurations;

	singleton->myUri.SetOwnUri();

	int i = 1;
	while( i < argc ){
		if( singleton->areEqual(argv[i], "--configFileName") ||
				  singleton->areEqual(argv[i], "-c")){
			if( i++ < argc ){
				singleton->configFilename.assign( argv[i] );
				singleton->moduleConfig->parseFromFile( argv[i] );
				singleton->handleConfig();
			}else{
				LogError( "Missing configFilename!" );
			}
		}else if( singleton->areEqual(argv[i], "--initialModuleUri") ){
			if( i++ < argc ){
				singleton->initialModuleUri.assign( argv[i] );
			}else{
				LogError( "Missing initialModuleUri!" );
			}
		}else if( singleton->areEqual(argv[i], "--spawnId") ){
			if( i++ < argc ){
				singleton->spawnId = atoi( argv[i] );
			}else{
				LogError( "Missing spawnId!" );
			}
		}else{
			LogWarning(" Un-recognized parameter %s !", argv[i]);
		}
		i++;
	}

	if( singleton->myUri.GetHttpPort() == Error::INVALID_PORT_NUM && !isMainModule ){
		LogError( "Missing serverPort!" );
	}
	if( singleton->myUri.GetTcpPort() == Error::INVALID_PORT_NUM && !isMainModule ){
		LogError( "Missing serverPort!" );
	}
	if( singleton->configFilename == "" ){
		LogWarning( "Missing configFilename!" );
	}
	if( singleton->spawnId == Error::INVALID_PARAM && !isMainModule ){
		LogError( "Missing spawnId!" );
	}

}

void ModuleSkeleton::createTcpPeerManager(int port){
	if( peers ){
		LogWarning("Tcp Peer Manager is already created.");
	}
	peers = PeerManager::GetInstance(port, & myUri);
	peers->SetJsonHandler(& ModuleSkeleton::handleTcpJson );
	myUri.SetTcpPort(port);
}

void ModuleSkeleton::createHttpServer(int port){
	if( httpServer ){
		LogWarning("HTTP Server is already created.");
	}
	httpServer = HTTPServer::GetInstance(port, & ModuleSkeleton::handleGet, & ModuleSkeleton::handlePost);
	myUri.SetHttpPort(port);
}

void ModuleSkeleton::createFileServer(string path, string subdomain){
	fileServer = FileServer::GetInstance(path, subdomain);
}

string ModuleSkeleton::handleGet(string url, string data){
	if( !singleton ){
		LogError("Module Skeleton is not initialized yet!");
		return SERVER_ERROR;
	}
	if( singleton->fileServer && singleton->fileServer->belongsToFileServer(url) ){
		return singleton->fileServer->handleGetRequest(url, data);
	}else if(singleton->getHandler){
		string result;
		singleton->getHandler(url, data, result);
		return result;
	}else{
		return NOT_IMPLEMENTED;
	}
}

string ModuleSkeleton::handlePost(string url, vector<pair<string, string> > param){
	if( !singleton ){
		LogError("Module Skeleton is not initialized yet!");
		return SERVER_ERROR;
	}
	if(singleton->getHandler){
		string result;
		singleton->postHandler(url, param, result);
		return result;
	}else{
		return NOT_IMPLEMENTED;
	}
}

void ModuleSkeleton::handleTcpJson(const JsonObject& json, ModuleUri& uri){
	if( !singleton ){
		LogError("Module Skeleton is not initialized yet!");
		return;
	}

	if(singleton->jsonHandler != NULL){
		singleton->jsonHandler(uri, json);
	}else{
		JsonObject jsonCpy = json;
		LogWarning("Dropping received TCP Json message from %s:\n%s",
				uri.GetFullString().c_str(), jsonCpy.ToString().c_str());
	}
}

bool ModuleSkeleton::haveConnectionTo(ModuleUri& uri){
	if( !peers ){
		LogError("Tcp Peer Manager is not created.");
		return false;
	}
	return peers->haveConnectionTo(uri);
}

bool ModuleSkeleton::createTcpConnectionTo(ModuleUri& uri, bool silent){
	if( !peers ){
		LogError("Tcp Peer Manager is not created.");
		return false;
	}
	if( peers->haveConnectionTo(uri) ){
		if(!silent) LogInfo("We are already connected to %s", uri.GetFullString().c_str());
		return true;
	}
	if(silent == false)
		LogInfo("Creating TCP connection to %s", uri.GetFullString().c_str());
	return peers->createConnectionTo(uri.GetHostName(), uri.GetTcpPort(), silent);
}

void ModuleSkeleton::spawnLocally(string binary, JsonObject* configJson){

	string cmd = "./" + binary;
	if(configJson != NULL){
		int randId = rand();
		string configFileName = "config_" + binary + "_" + to_string(randId) + ".json";
		Pagedownloader::writeTextToFile(configJson->ToString(), configFileName);
		cmd += " -c ./" + configFileName;
	}
	cmd += " &";
	LogInfo("Spawning with cmd = %s", cmd.c_str());
	Bash::GetInstance()->executeBlockingSystemCommand(cmd);
}

bool ModuleSkeleton::sendJson(ModuleUri& toUri, JsonObject& json){
	if( !peers ){
		LogError("Tcp Peer Manager is not created.");
		return false;
	}
	return peers->sendJson(toUri, json);
}

bool ModuleSkeleton::waitFor(const ModuleUri& fromUri, const string& field, const string& val, JsonObject& replyJson, int time0){
	if( !peers ){
		LogError("Tcp Peer Manager is not created.");
		return false;
	}
	return peers->waitFor(fromUri, field, val, replyJson, time0);
}

void ModuleSkeleton::sendGetRequest(const string& url, string& result){
	result = httpClient->postRequest(url.c_str(), "", HTTPClient::METHOD_GET);
}

void ModuleSkeleton::SignalHandler(int signum){

	LogError("Seg Handler for signal %s", strsignal(signum));
	if(singleton->interrupthandler != NULL){
		singleton->interrupthandler(signum);
	}

	exit(0);
}

void ModuleSkeleton::SetSignalHandler(){
	signal(SIGSEGV , ModuleSkeleton::SignalHandler);
	signal(SIGFPE , ModuleSkeleton::SignalHandler);
	signal(SIGILL , ModuleSkeleton::SignalHandler);
	signal(SIGINT , ModuleSkeleton::SignalHandler);
	signal(SIGTERM , ModuleSkeleton::SignalHandler);
	signal(SIGABRT , ModuleSkeleton::SignalHandler);
}

int ModuleSkeleton::GetServerPort(){
	if(httpServer == NULL) return Error::INVALID_PORT_NUM;
	return httpServer->GetPort();
}


