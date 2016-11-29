/*
 * ModuleSkeleton.h
 *
 *  Created on: Jul 8, 2016
 *      Author: mahdy
 */

#ifndef MODULE_MODULESKELETON_H_
#define MODULE_MODULESKELETON_H_

#include <string>
#include <signal.h>
#include <types/ModuleUri.h>
#include <types/JsonObject.h>
#include <http/FileServer.h>
#include <http/HTTPServer.h>
#include <http/HTTPClient.h>
#include <utils/Configurations.h>
#include <Module/PeerManager.h>

using namespace std;

class ModuleSkeleton {
public:
	typedef void (GetHandler) (const string& url, const string& data, string& output);
	typedef void (PostHandler) (const string& url, const vector<pair<string, string> > params, string& output);
	typedef void (JsonHandler) (const ModuleUri& module, const JsonObject& inJson);
	typedef void (InterruptSignalHandler) (int signal);
	typedef void (PeerDisconnectHandler) (const ModuleUri& uri);
	static const int UNDEFINED_SERVER_PORT;
	static const string JSON_HANDLING_ERROR;
	static const string NOT_IMPLEMENTED;
	static const string SERVER_ERROR;
private:

	PeerManager* peers;

	Configurations* moduleConfig;
	string configFilename;
	ModuleUri initialModuleUri;
	int spawnId;

	bool isMainModule;
	ModuleUri myUri;
	static ModuleSkeleton* singleton;

	string fileServerSubDomain;
	string fileServerRootPath;

	HTTPServer* httpServer;
	HTTPClient* httpClient;
	FileServer* fileServer;

	GetHandler* getHandler;
	PostHandler* postHandler;
	JsonHandler* jsonHandler;
	InterruptSignalHandler* interrupthandler;

	ModuleSkeleton();

	inline bool areEqual(const char* str1, const char* str2){return strcmp(str1, str2) == 0; };

	void handleConfig();

	Logging* logger;

public:

	virtual ~ModuleSkeleton();

	static void initSkeleton(int argc, char** argv, bool isMainModule = false);
	static ModuleSkeleton* GetInstance(){return singleton;};
	void createHttpServer(int port);
	void createTcpPeerManager(int port);
	void createFileServer(string path = "./", string subdomain = "fileserver");
	void setGetHandler(GetHandler* handler){this->getHandler = handler;};
	void setPostHandler(PostHandler* handler){this->postHandler = handler;};
	void setJsonHandler(JsonHandler* handler){this->jsonHandler = handler;};
	void setInterruptHandler(InterruptSignalHandler* handler){this->interrupthandler = handler;};
	void setPeerDisconnectHandler(PeerDisconnectHandler* handler){if(peers) peers->SetPeerDisconnectHandler(handler);};

	static string handleGet(string url, string data);
	static string handlePost(string url, vector<pair<string, string> > params);
	static void handleTcpJson(const JsonObject& json, ModuleUri& uri);

	void spawnLocally(string binary, JsonObject* configJson = NULL);
	bool sendJson(ModuleUri& toUri, JsonObject& json);
	bool waitFor(const ModuleUri& fromUri, const string& field, const string& val, JsonObject& replyJson, int time0 = 100);
	bool createTcpConnectionTo(ModuleUri& uri, bool silent = false);
	bool haveConnectionTo(ModuleUri& uri);
	void sendGetRequest(const string& url, string& result);

	FileServer* GetFileServer(){return fileServer;};
	Configurations* GetConfigurations(){return moduleConfig;};
	JsonObject* GetConfigJson(){return moduleConfig->GetJson();};

	static void SignalHandler(int signum);
	static void SetSignalHandler();

	ModuleUri* GetOwnUri(){return & this->myUri;};
	int GetServerPort();
	void KillMe(){exit(1);};
};

#endif /* MODULE_MODULESKELETON_H_ */
