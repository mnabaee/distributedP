/*
 * Pagedownloader.h
 *
 *  Created on: Jul 19, 2016
 *      Author: mahdy
 */

#ifndef UTILS_PAGEDOWNLOADER_H_
#define UTILS_PAGEDOWNLOADER_H_

#include <http/HTTPClient.h>
#include <types/ModuleUri.h>
#include <Module/ModuleSkeleton.h>

#include <string>
using namespace std;
class Pagedownloader {

	HTTPClient* httpClient;

	ModuleUri* pythonDownloaderUri;
	int pythonDownloaderTimeout;
	unsigned long curlTimeout;
	ModuleSkeleton* moduleSk;

public:
	Pagedownloader();
	virtual ~Pagedownloader();

	void downloadPageHttp(const string& fullUri, string& pageContent, const string& filename = "");
	static bool readTextFile(const string& filename, string& content);
	static bool writeTextToFile(const string& content, const string& filename);
	static bool fileExists(const string& filename);

	bool initPythonDownloader(JsonObject& jsonCfg, ModuleSkeleton* m);
	void initPythonDownloader(string hostname, int port, ModuleSkeleton* m, int timeout = 10000);
	void SetCurlTimeout(unsigned long timeout){curlTimeout = timeout;};
	//This is a blocking request to python server for downloading a uri
	bool requestPythonDownloader(const string& fullUri, string& pageContent, const string& filename = "");
	bool pythonDownloaderReady();
	bool makePythonDownloaderReady();

};

#endif /* UTILS_PAGEDOWNLOADER_H_ */
