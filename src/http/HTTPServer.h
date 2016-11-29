/*
 * HTTPServer.h
 *
 *  Created on: Jan 15, 2016
 *      Author: mahdy
 */

#ifndef SRC_COMM_HTTPSERVER_H_
#define SRC_COMM_HTTPSERVER_H_

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <utility>

#include <types/JsonObject.h>


#define POSTBUFFERSIZE  512
#define MAXNAMESIZE     20
#define MAXANSWERSIZE   512

#define GET             0
#define POST            1
#define POST_DATA_FIELD "CONTENTS"

class HTTPServer{
public:
	typedef std::string (RequestHandler) (string url, string data);
	typedef std::string (PostHandler) (string url, vector<pair<string, string> > params);
private:
	int serverPort;
	struct MHD_Daemon *daemon;
	static HTTPServer* mSingleton;
	int init();
	int stop();
	HTTPServer(int port);

	static int	send_page (struct MHD_Connection *connection, const char *page);

	static int
	iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
	              const char *filename, const char *content_type,
	              const char *transfer_encoding, const char *data, uint64_t off,
	              size_t size);

	static void
	request_completed (void *cls, struct MHD_Connection *connection,
	                   void **con_cls, enum MHD_RequestTerminationCode toe);

	static int
	answer_to_connection (void *cls, struct MHD_Connection *connection,
	                      const char *url, const char *method,
	                      const char *version, const char *upload_data,
	                      size_t *upload_data_size, void **con_cls);

	static RequestHandler* handleGet;
	static PostHandler* handlePost;


public:
	~HTTPServer();
	static HTTPServer* GetInstance(int port, RequestHandler* handleGet, PostHandler* handlePost);

	struct connection_info_struct
	{
	  vector< pair<string, string> >* params;
	  int connectiontype;
	  char *answerstring;
	  struct MHD_PostProcessor *postprocessor;
	  char* urlStr;
	};

	static const char *askpage;
	static const char *greetingpage;
	static const char *errorpage;
	static const char *notimplemented;

	int GetPort(){return serverPort;};

};


#endif /* SRC_COMM_HTTPSERVER_H_ */
