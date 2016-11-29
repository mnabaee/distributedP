/*
 * HTTPServer.cpp
 *
 *  Created on: Jan 15, 2016
 *      Author: mahdy
 */

#include "HTTPServer.h"
#include "utils/Logging.h"

const char * HTTPServer::askpage = "<html><body>\
					   What's your name, Sir?<br>\
					   <form action=\"/namepost\" method=\"post\">\
					   <input name=\"name\" type=\"text\">\
					   <input type=\"submit\" value=\" Send \"></form>\
					   </body></html>";

const char * HTTPServer::greetingpage =
  "<html><body><h1>Welcome, %s!</center></h1></body></html>";

const char * HTTPServer::errorpage =
  "<html><body>This doesn't seem to be right.</body></html>";

const char * HTTPServer::notimplemented =
  "<html><body>The method is not implemented!</body></html>";

HTTPServer* HTTPServer::mSingleton = NULL;
HTTPServer::RequestHandler* HTTPServer::handleGet = NULL;
HTTPServer::PostHandler* HTTPServer::handlePost = NULL;

HTTPServer::HTTPServer(int port){
	serverPort = port;
	daemon = NULL;
}

HTTPServer* HTTPServer::GetInstance(int port, RequestHandler* handleGet, PostHandler* handlePost){
	if(mSingleton == NULL){
		mSingleton = new HTTPServer(port);

		mSingleton->handleGet = handleGet;
		mSingleton->handlePost = handlePost;
		mSingleton->init();
	}
	return mSingleton;
}

HTTPServer::~HTTPServer(){
	//Since mq is a static member, it can/should not be deleted...
	stop();
}

int HTTPServer::init(){

	  daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY , serverPort, NULL, NULL,
              &answer_to_connection, NULL,
              MHD_OPTION_NOTIFY_COMPLETED, request_completed,
              NULL, MHD_OPTION_END);

	  if (NULL == daemon){
		  LogError("Failed to start MHD Daemon with port %d", serverPort);
		  return 0;
	  }else{
		  LogInfo("MHD Daemon is initiated to port %d", serverPort);
		  return 1;
	  }

}

int HTTPServer::stop(){
	if(daemon != NULL){
	  MHD_stop_daemon (daemon);
	  LogInfo("MHD Daemon is stopped!");
	  return 1;
	}else{
	  LogWarning("MHD Daemon is not stopped!");
		return 0;
	}
}

int HTTPServer::send_page (struct MHD_Connection *connection, const char *page)
{
  int ret;
  struct MHD_Response *response;

  response =
    MHD_create_response_from_buffer (strlen (page), (void *) page,
    		MHD_RESPMEM_MUST_COPY );


  if (!response)
    return MHD_NO;


  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);
  return ret;
}

int HTTPServer::iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
        const char *filename, const char *content_type,
        const char *transfer_encoding, const char *data, uint64_t off,
        size_t size)
{
struct connection_info_struct *con_info = (struct connection_info_struct *) coninfo_cls;
string keyStr, valStr;
if(key){
	keyStr.assign(key);
}
if(data){
	valStr.assign(data);
}
if(con_info && con_info->params){
	con_info->params->push_back(make_pair(keyStr, valStr));
	return MHD_YES;
}else{
	return MHD_NO;
}
/*
if (0 == strcmp (key, POST_DATA_FIELD))
{
    string urlStr, dataStr;
    if(data){
    	dataStr.assign(data);
    }
    urlStr.assign(con_info->urlStr);
    string postStr = handlePost(urlStr, dataStr);
    char *answerstring = (char*) malloc( postStr.size() );

    strcpy(answerstring, postStr.c_str());

    con_info->answerstring = answerstring;

    return MHD_NO;
}else
	return MHD_YES;
*/
}


void HTTPServer::request_completed (void *cls, struct MHD_Connection *connection,
        void **con_cls, enum MHD_RequestTerminationCode toe)
{
struct connection_info_struct *con_info = (struct connection_info_struct *) *con_cls;

if (NULL == con_info)
return;

if (con_info->connectiontype == POST)
{
MHD_destroy_post_processor (con_info->postprocessor);
if (con_info->answerstring)
free (con_info->answerstring);
}
if(con_info->params){
delete con_info->params;
}

free (con_info);
*con_cls = NULL;
}

int
HTTPServer::answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **con_cls)
{
  if (NULL == *con_cls)
    {
      struct connection_info_struct *con_info;

      con_info = (struct connection_info_struct *) malloc (sizeof (struct connection_info_struct));
      if (NULL == con_info)
        return MHD_NO;
      con_info->answerstring = NULL;
      //con_info->params = (vector< pair<string, string> >*) malloc(sizeof(vector< pair<string, string> >));
      con_info->params = new vector< pair<string, string> >;

      if (0 == strcmp (method, "POST"))
        {
          con_info->postprocessor =
            MHD_create_post_processor (connection, POSTBUFFERSIZE,
                                       iterate_post, (void *) con_info);

          if (NULL == con_info->postprocessor)
            {
              free (con_info);
              return MHD_NO;
            }

          con_info->connectiontype = POST;
        }
      else
        con_info->connectiontype = GET;

      *con_cls = (void *) con_info;

      return MHD_YES;
    }

  if (0 == strcmp (method, "GET"))
    {

	  string urlStr, dataStr, formatStr;
	  if(url) urlStr.assign(url);
	  if(upload_data){dataStr.assign(upload_data); dataStr = dataStr.substr(0, *upload_data_size);}

	  string getString = handleGet(urlStr, dataStr);

      return send_page (connection, getString.c_str());
    }

  if (0 == strcmp (method, "POST"))
    {
      struct connection_info_struct *con_info = (struct connection_info_struct *) *con_cls;
      if( url && con_info){
		  con_info->urlStr = (char*) malloc(sizeof(url));
		  strcpy(con_info->urlStr, url);
      }
      if (*upload_data_size != 0)
        {
          MHD_post_process (con_info->postprocessor, upload_data,
                            *upload_data_size);
          *upload_data_size = 0;

          return MHD_YES;
        }
      else{// if (NULL != con_info->answerstring)
    	  string res = notimplemented;
    	  if( handlePost ){
    		  vector<pair<string, string> > paramsCpy = * con_info->params;
    		  res = handlePost( url, paramsCpy );
    	  }
    	  con_info->answerstring = (char*) malloc(sizeof("DONE"));

        return send_page (connection, res.c_str());
      }
    }

  return send_page (connection, errorpage);
}



