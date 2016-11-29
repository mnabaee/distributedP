/*
 * Pagedownloader.cpp
 *
 *  Created on: Jul 19, 2016
 *      Author: mahdy
 */

#include "Pagedownloader.h"
#include <utils/Bash.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <iostream>
#include <Module/ModuleSkeleton.h>
#include <messages/RequestPythonDownloader.h>
#include <messages/ReplyPythonDownloader.h>
#include <utils/StringModifier.h>

Pagedownloader::Pagedownloader() {
	httpClient = new HTTPClient;
	if(httpClient == NULL){
		LogFatal("HttpClient object could not be created!");
	}
	pythonDownloaderUri = NULL;
	pythonDownloaderTimeout = 10000;
	moduleSk = NULL;
	curlTimeout = 10;
}

Pagedownloader::~Pagedownloader() {
	delete pythonDownloaderUri;
	delete httpClient;
}

void Pagedownloader::downloadPageHttp(const string& fullUrl, string& pageContent, const string& filename){
	if(httpClient != NULL){
		pageContent = httpClient->postRequest(fullUrl.c_str(), "", HTTPClient::METHOD_GET, curlTimeout);
	}
	if(filename != ""){
		writeTextToFile(pageContent, filename);
	}
}

bool Pagedownloader::readTextFile(const string& filename, string& content){
	if( fileExists(filename) ){
		std::ifstream t(filename);
		std::string str((std::istreambuf_iterator<char>(t)),
	                 std::istreambuf_iterator<char>());

		content = str;

		return true;
	}else{
		return false;
	}
}

bool Pagedownloader::fileExists(const string& filename){
    struct stat buf;
    return (stat(filename.c_str(), &buf) == 0);
}

bool Pagedownloader::writeTextToFile(const string& content, const string& filename){
	 std::ofstream out(filename);
	    out << content;
	    out.close();
	    return true;
}

bool Pagedownloader::pythonDownloaderReady(){
	return (pythonDownloaderUri && moduleSk &&
			moduleSk->haveConnectionTo(*pythonDownloaderUri));
}

bool Pagedownloader::initPythonDownloader(JsonObject& jsonCfg, ModuleSkeleton* m){
	if( jsonCfg.hasValue("Hostname") &&
		jsonCfg.hasValue("TcpPort") &&
		jsonCfg.hasValue("ResponseTimeout") &&
		m != NULL ){
		initPythonDownloader( jsonCfg.GetString("Hostname"),
							  jsonCfg.GetInt("TcpPort"),
							  m,
							  jsonCfg.GetInt("ResponseTimeout") );
		return true;
	}else{
		LogError("could not initialize page downloader!");
		return false;
	}
}

void Pagedownloader::initPythonDownloader(string hostname, int port, ModuleSkeleton* m, int timeout){
	moduleSk = m;
	pythonDownloaderUri = new ModuleUri;
	pythonDownloaderUri->SetHostName(hostname);
	pythonDownloaderUri->SetTcpPort(port);
	pythonDownloaderTimeout = timeout;
	if(ModuleSkeleton::GetInstance()->createTcpConnectionTo(*pythonDownloaderUri)){
		LogInfo("Successfully initialized Python Downloader.");
	}else{
		LogError("could not initialize python downloader - missing connection to python server!");
	}
}

bool Pagedownloader::makePythonDownloaderReady(){
	if( pythonDownloaderUri == NULL ||
		moduleSk == NULL){
		return false;
	}
	return moduleSk->createTcpConnectionTo(*pythonDownloaderUri, true);
}

bool Pagedownloader::requestPythonDownloader(const string& fullUri, string& pageContent, const string& filename){
	if(pythonDownloaderUri == NULL || moduleSk == NULL ||
		!moduleSk->haveConnectionTo(*pythonDownloaderUri)){
		LogError("Python Downloader is not initialized yet!");
		return false;
	}
	string fullUriBase64encoded = StringModifier::base64Encode( fullUri );
	//string decoded_ = StringModifier::base64Decode(fullUriBase64encoded);
	//LogInfo("encoding \n%s\n%s\n%s", fullUri.c_str(), decoded_.c_str(), fullUriBase64encoded.c_str());
	JsonObject reqMsg = RequestPythonDownloader::create(fullUriBase64encoded, pythonDownloaderTimeout / 1000);
	if(!moduleSk->sendJson(*pythonDownloaderUri, reqMsg)){
		pageContent = "";
		return false;
	}
	JsonObject replyMsg;
	if(!moduleSk->waitFor(*pythonDownloaderUri, "message_type", ReplyPythonDownloader::msgType(), replyMsg, pythonDownloaderTimeout)){
		pageContent = "";
		return false;
	}
	string rxUri;
	if( ReplyPythonDownloader::parse(replyMsg, rxUri, pageContent) &&
		rxUri == fullUri && pageContent.length() > 1){
		if(filename != ""){
			writeTextToFile(pageContent, filename);
		}
		return true;
	}else return false;
}
