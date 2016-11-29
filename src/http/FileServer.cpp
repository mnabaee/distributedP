/*
 * FileServer.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: mahdy
 */

#include "FileServer.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <streambuf>

#define NOT_FOUND "<!DOCTYPE html><html><head><title>File not found</title></head><body>File not found</body></html>"
#define EMPTY_PAGE "<!DOCTYPE html><html><head><title>%s</title></head><body>%s</body></html>"
#define LINK "<a href=\"%s\">%s</a><br>"

FileServer* FileServer::singleton = NULL;

FileServer::FileServer(std::string rootpath, string subdomain_) {
	rootPath = rootpath;
	subdomain = subdomain_;
	LogInfo("Created File Server for rootpath=%s, subdomain=%s", rootpath.c_str(), subdomain.c_str());

}

FileServer::~FileServer() {
	// TODO Auto-generated destructor stub
}

bool FileServer::belongsToFileServer(const string& url){
	return hasSubUrl( url );
}

string FileServer::getSubUrl(const string& url){
	int index = url.find( subdomain );
	if( index < 0 ){
		LogWarning(" could not find the subdomain [%s] in url [%s] ", subdomain.c_str(), url.c_str());
		return url;
	}
	return url.substr( index + subdomain.length() );
}

bool FileServer::hasSubUrl(const string& url){
	int index = url.find( subdomain );
	return index >= 0;
}

string FileServer::handleGetRequest(string url, string data){
	if(!singleton){
		LogError("File Server singleton is not initialized!");
		return NOT_FOUND;
	}
	if( ! singleton->hasSubUrl(url) ){
		return NOT_FOUND;
	}

	string path = singleton->rootPath + singleton->getSubUrl(url);
	string result;
	if( singleton->isDir(path) ){
		vector<string> elements;
		singleton->lsDir(path, elements);
		string url_ = url;
		if( url.substr(url.length() -1) == "/"){
			url_.erase(url_.length() - 1);
		}
		singleton->convertDirListToHtml(url_ + "/", elements, result);
	}else if( singleton->fileExists(path) ){
		singleton->readTextFile(path, result);
	}else{
		return NOT_FOUND;
	}
	string html;
	singleton->fillInHtml(url, result, html);
	return result;
}

bool FileServer::lsDir(string dirStr, vector<string> & result){
	result.clear();
	DIR *dir;
	if ((dir = opendir ( dirStr.c_str() )) != NULL) {
     	struct dirent *ent;
	  while ((ent = readdir (dir)) != NULL) {
		  result.push_back( ent->d_name );
	  }
	  closedir (dir);
	  return true;
	} else {
		return false;
	}
}

bool FileServer::isDir(string dirStr){
	DIR *dir;
	if ((dir = opendir ( dirStr.c_str() )) != NULL) {
	  closedir (dir);
	  return true;
	} else {
	  return false;
	}
}

bool FileServer::fileExists(const string& filename){
    struct stat buf;
    return (stat(filename.c_str(), &buf) == 0);
}

bool FileServer::readTextFile(const string& filename, string& content){
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

void FileServer::convertDirListToHtml(string preDomain, vector<string>& list, string& html){

	//html = "<ul>\n";
	for(auto &elem : list){
		string linkAddress = preDomain + elem;
		int size = strlen(LINK) + elem.length() + linkAddress.length() + 10;
		char* str = (char*) malloc(size);
		if(str == NULL){
			LogError(" could not allocate char of size %d", size);
			return;
		}
		sprintf(str, LINK, linkAddress.c_str(), elem.c_str());
		//html += "<li>";
		html += str;
		//html += "</li>\n";
		free(str);
	}
	//html += "</ul>";
	return;

}

void FileServer::fillInHtml(const string& title, const string& body, string& html){
	int size = strlen(EMPTY_PAGE) + title.length() + body.length();
	char * result = (char*) malloc(size);
	if( result == NULL){
		LogError(" could not allocate char of size %d", size);
		return;
	}
	sprintf(result, EMPTY_PAGE, title.c_str(), body.c_str());
	html.assign( result );
	free(result);
	return;
}

void FileServer::saveStrToFile(const string & filename, const string& content){
    std::ofstream out(filename);
    out << content;
    out.close();
}


