/*
 * ConnectionManager.h
 *
 *  Created on: Aug 25, 2016
 *      Author: root
 */

#ifndef PEERMANAGER_H_
#define PEERMANAGER_H_

#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <types/JsonObject.h>
#include <types/ModuleUri.h>
#include <list>
#include <chrono>

using namespace std;

#define LISTEN_BACKLOG_LEN 5
#define READ_BUFFER_LEN 1024
#define HEART_BEAT_PERIOD 60

class PeerManager {

	static PeerManager* singleton;
	static int peerIdCounter;

	void (*peerDisconnectHandler)(const ModuleUri& uri);

	ModuleUri* ownUri;
	struct Peer{
		enum{
			CLIENT,
			SERVER
		};
		int sockfd;
		int side;
		int id;
		ModuleUri uri;
	};
	string getSideString(int side){
		switch(side){
		case Peer::CLIENT: return "CLIENT";
		case Peer::SERVER: return "SERVER";
		default: return "INVALID";
		}
	};

	int own_sockfd;
	int own_port;
	struct sockaddr_in own_addr;

	vector<Peer> peers;
	mutex mutex_peers;

	void initOwnSocket(int port);

	static void waitForPeersStatic(){return singleton->waitForPeers();};
	void waitForPeers();
	static void receiveStatic(int sockfd, int side, string hostname, int port, int* isDone){return singleton->receive(sockfd, side, hostname, port, isDone);};
	void receive(int sockfd, int side, string hostname, int port, int* isDone);

	int send(int sockfd, string& str);

	ModuleUri addPeer(int side, int sockfd, string moduleUriStr, string hostname = "", int port = 0);
	void DelPeer(ModuleUri &uri);

	static const string preStringBeacon, postStringBeacon;

	mutex mutex_messages;
	condition_variable cv_messages;
	list< pair<JsonObject, ModuleUri> > messages;
	static void messageHandler();
	PeerManager();
	void (*mJsonHandler)(const JsonObject& json, ModuleUri& uri);
	int sendRegisterMessage(int sockfd);

	bool isWaitResponse(JsonObject& replyJson, ModuleUri& replyUri);
	bool isWaitResponse(const ModuleUri& fromUri, const string& field, const string& val, JsonObject& replyJson, ModuleUri& replyUri);
	struct waitForStruct{
		ModuleUri fromUri;
		string field;
		string val;
		JsonObject* jsonReply;
		waitForStruct(){
			jsonReply = NULL;
		}
	};
	waitForStruct* amWaitingFor;

	class HeartBeatMessage{
	public:
		static string MsgType(){return "heart_beat";};
		static JsonObject create(){
			JsonObject res;
			res.PutString("message_type", MsgType());
			return res;
		};
		static bool is(JsonObject& json){
			if( json.hasValue("message_type") &&
				json.GetString("message_type") == MsgType()){
				return true;
			}else return false;
		};
	};
	static void sendHearBeat(int sockfd);

public:

	typedef void (JsonHandlerFunc)(const JsonObject& json, ModuleUri& uri);
	static const string REGISTER_MSG;

	static PeerManager* GetInstance(int port, ModuleUri* uri);
	void SetJsonHandler(JsonHandlerFunc* funcPtr){mJsonHandler = funcPtr;};
	void SetPeerDisconnectHandler(void (*handler)(const ModuleUri& uri) ){ peerDisconnectHandler = handler; };
	virtual ~PeerManager();

	bool createConnectionTo(string hostname, int port, bool silent = false);
	bool sendJson(ModuleUri& toUri, const JsonObject& json);
	bool waitFor(const ModuleUri& fromUri, const string& field, const string& val, JsonObject& replyJson, int time0);
	vector<ModuleUri> getListOfPeers();
	bool haveConnectionTo(ModuleUri& uri);

};

#endif /* PEERMANAGER_H_ */
