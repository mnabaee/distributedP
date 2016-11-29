/*
 * FileServer.h
 *
 *  Created on: Jul 8, 2016
 *      Author: mahdy
 */

#ifndef HTTP_FILESERVER_H_
#define HTTP_FILESERVER_H_

#include <string>
#include <vector>

using namespace std;

#include <http/HTTPServer.h>

class FileServer {


	FileServer(string rootPath = "./", string subdomain = "");

	static FileServer* singleton;
	string rootPath;
	string subdomain;

	bool lsDir(string dirStr, vector<string> & result);
	bool isDir(string dirStr);
	bool fileExists(const string& filename);

	string getSubUrl(const string& url);
	bool hasSubUrl(const string &url);

	void convertDirListToHtml(string preDomain, vector<string>& list, string& html);

	void fillInHtml(const string& title, const string& body, string& html);

public:
	static FileServer* GetInstance(string rootPath = "./", string subdomain = ""){
		if(!singleton){
			singleton = new FileServer(rootPath, subdomain);
		}
		return singleton;
	};

	virtual ~FileServer();

	static string handleGetRequest(string url, string data);

	bool belongsToFileServer(const string& url);
	void saveStrToFile(const string & filename, const string& content);
	bool readTextFile(const string& filename, string& content);


};

#endif /* HTTP_FILESERVER_H_ */
