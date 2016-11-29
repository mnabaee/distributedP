/*
 * ConnectionManager.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: root
 */

#include "PeerManager.h"
#include <utils/Error.h>
#include <utils/Logging.h>
#include <messages/QuitModule.h>
#include <Module/ModuleSkeleton.h>
#include <signal.h>

PeerManager* PeerManager::singleton = NULL;
int PeerManager::peerIdCounter = 0;
const string PeerManager::REGISTER_MSG = "Register_Message";

const string PeerManager::preStringBeacon = "OO__BEACON__STRING__BEGIN__OO";
const string PeerManager::postStringBeacon = "OO__BEACON__STRING__END__OO";

ModuleUri PeerManager::addPeer(int side, int sockfd, string moduleUriStr, string hostname, int port){
	Peer newPeer;
	newPeer.side = side;
	newPeer.sockfd = sockfd;
	ModuleUri uri;
	if(!newPeer.uri.assign(moduleUriStr)){
		LogWarning("Failed to parse module uri string %s", moduleUriStr.c_str());
	}
	if(hostname != ""){
		newPeer.uri.SetHostName(hostname);
	}
	if(port != 0){
		newPeer.uri.SetTcpPort(port);
	}
	mutex_peers.lock();
		newPeer.id = peerIdCounter;
		peerIdCounter++;
		peers.push_back(newPeer);
	mutex_peers.unlock();
	LogInfo("a new peer is added with id=%d, sockfd=%d, side=%s, uri=%s", newPeer.id, newPeer.sockfd, getSideString(newPeer.side).c_str(), newPeer.uri.GetFullString().c_str());

	return newPeer.uri;
}

void PeerManager::DelPeer(ModuleUri& uri){
	mutex_peers.lock();
		for( auto it = peers.begin(); it != peers.end(); it++ ){
			if( it->uri.isTheSame(uri) ){
				LogInfo("Connection to peer with uri=%s was gracefully closed.", uri.GetFullString().c_str());
				peers.erase(it);
				mutex_peers.unlock();
				return;
			}
		}
		LogWarning("Could not find peer corresponding to uri=%s", uri.GetFullString().c_str());
	mutex_peers.unlock();
}

PeerManager::PeerManager() {
	own_sockfd = -1;
	own_port = -1;
	mJsonHandler = NULL;
	srand(time(NULL));
	ownUri = NULL;
	peerDisconnectHandler = NULL;
	amWaitingFor = NULL;
}

PeerManager::~PeerManager() {
	// TODO Auto-generated destructor stub
}

PeerManager* PeerManager::GetInstance(int port, ModuleUri* uri){
	if( singleton == NULL ){
		singleton = new PeerManager;
		singleton->initOwnSocket(port);
		singleton->ownUri = uri;
	}
	return singleton;
}

void PeerManager::initOwnSocket(int port){
	own_port = port;
	own_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(own_sockfd < 0){
		perror("can not create a socket for ourself!");
		exit(1);
	}

	bzero((char *) &own_addr, sizeof(own_addr));

	own_addr.sin_family = AF_INET;
	own_addr.sin_addr.s_addr = INADDR_ANY;
	own_addr.sin_port = htons(own_port);
    if (bind(own_sockfd, (struct sockaddr *) &own_addr,
             sizeof(own_addr)) < 0){
             perror("ERROR on binding to por!");
             exit(1);
    }
    LogInfo("Initiated the socket for port %d fd=%d", own_port, own_sockfd);

    thread tWait(& PeerManager::waitForPeersStatic);
    tWait.detach();

    thread tMessHandler(& PeerManager::messageHandler);
    tMessHandler.detach();

}

void PeerManager::waitForPeers(){
	struct sockaddr_in peer_addr;
    listen(own_sockfd, LISTEN_BACKLOG_LEN);
    socklen_t peer_len = sizeof(peer_addr);

    LogInfo("waiting for peers...");

    while(true){
		int newsockfd = accept(own_sockfd,
					(struct sockaddr *) &peer_addr,
					&peer_len);
		if (newsockfd < 0){
			LogError("could not accept socket!");
			continue;
		}

		sendRegisterMessage(newsockfd);
		int* isDone = new int;
		thread thread_( & PeerManager::receiveStatic, newsockfd, Peer::SERVER, "", 0, isDone );
		thread_.detach();

		//LogInfo("accepted a new peer connection with sockfd=%d", newsockfd);

    }
}

void PeerManager::receive(int sockfd, int side, string hostname, int port, int* isDone){

	string fullBuffer = "";
	bool flagClosed = false;
	ModuleUri thisPeeruri;
	bool registered = false;

	while(true){
		 char buffer[READ_BUFFER_LEN];
		 int bytes_received = 0;

		 do{
			 bzero(buffer,READ_BUFFER_LEN);
			 bytes_received = recv(sockfd, buffer, READ_BUFFER_LEN-1, 0);
			 if( bytes_received < 0 ){
				 LogError("Error reading from socket fd=%d", sockfd);
				 flagClosed = true;
				 break;
				 //exit(1);
			 }else if( bytes_received == 0 ){
				 //Connection is gracefully closed
				 flagClosed = true;
				 break;
			 }else if(bytes_received > 0 && bytes_received == READ_BUFFER_LEN){
				 fullBuffer.append(buffer);
			 }else if(bytes_received > 0 && bytes_received < READ_BUFFER_LEN){
				 fullBuffer.append(buffer);
				 break;
			 }
		 }while(true);

		 if(flagClosed){
			 //LogInfo("Connection with sockfd = %d is gracefully closed.\n", sockfd);
			 DelPeer(thisPeeruri);
			 delete isDone;
			 if(peerDisconnectHandler){
				 peerDisconnectHandler(thisPeeruri);
			 }
			 return;
		 }

		 size_t index0, index1;
		 do{
			 index0 = fullBuffer.find(preStringBeacon);
			 if(index0 != string::npos){
				 index1 = fullBuffer.find( postStringBeacon, index0 + preStringBeacon.length() );
				 if( index1 != string::npos ){
					 string newMessageString = fullBuffer.substr( index0 + preStringBeacon.length(), index1 - index0 - preStringBeacon.length() );
					 //LogInfo("got str = %s", newMessageString.c_str());
					 JsonObject newJson;
					 if(JsonObject::ParseFromString(newMessageString, newJson)){
						 //LogInfo("ee = %s", newMessageString.c_str());
						 if(HeartBeatMessage::is(newJson)){
							 //Just ignore it.
						 }else if(newJson.hasValue("message_type") &&
								 newJson.GetString("message_type") == REGISTER_MSG &&
								 newJson.hasValue("module_uri") && !registered){
							 thisPeeruri = addPeer(side, sockfd, newJson.GetString("module_uri"), hostname, port);
							 registered = true;
							 if(isDone != NULL){
								 *isDone = 1;
							 }
							 thread heartBeatThread(& PeerManager::sendHearBeat, sockfd);
							 heartBeatThread.detach();

						 }else{
							 if(!registered){
								 LogError("The client has not registered yet!");
							 }else{
								 if( QuitModule::is( newJson ) ){
									 LogInfo("Quitting because of request from outside!");
									 ModuleSkeleton::GetInstance()->KillMe();
								 }else{
									 lock_guard<mutex> lock_messages(mutex_messages);
									 //LogInfo("adding msg %d", messages.size());
									 if( isWaitResponse(newJson, thisPeeruri) ){
										 amWaitingFor->jsonReply = new JsonObject;
										 if(amWaitingFor->jsonReply == NULL){
											 LogError("could not allocate memory!");
										 }else{
											 newJson.copyTo( * amWaitingFor->jsonReply );
										 }
									 }else{
										 messages.push_back(make_pair(newJson, thisPeeruri));
									 }
									 cv_messages.notify_all();
								 }
							 }
						 }
					 }else{
						 LogWarning("String can not be parsed into Json!\n%s", newMessageString.c_str());
					 }

					 fullBuffer = fullBuffer.substr( index1 + postStringBeacon.length() );
				 }
			 }
		 }while( index0 != string::npos && index1 != string::npos );

	}
}

void PeerManager::sendHearBeat(int sockfd){
	unsigned long long last_ = 0;
	JsonObject heartBeatMsg = HeartBeatMessage::create();
	string heartBeatStr = heartBeatMsg.ToString();
	while( true ){
		if( current_date_time_msec() - last_ >= HEART_BEAT_PERIOD * 1000 ){
			if(singleton == NULL || singleton->send(sockfd, heartBeatStr) != Error::NO_ERROR) return;
			last_ = current_date_time_msec();
		}
		sleep(1);
	}
}

bool PeerManager::createConnectionTo(string hostname, int port, bool silent){
	//LogInfo("creating TCP connection To %s:%d", hostname.c_str(), port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
    	LogError("Error Opening Socket!");
        return false;
    }

	int optval;
	socklen_t optlen = sizeof(optval);

   /* Set the option active */
   optval = 1;
   optlen = sizeof(optval);
   if(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
	   LogError("Error setting socket option.");
	  close(sockfd);
	  return false;
   }

    struct sockaddr_in serv_addr;
    struct hostent *server;
    server = gethostbyname(hostname.c_str());
    if (server == NULL) {
    	LogError("Invalid hostname = %s", hostname.c_str());
    	return false;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        if(!silent) LogError("Error connecting to %s:%d", hostname.c_str(), port);
        return false;
    }
    sendRegisterMessage(sockfd);
    int* isDone = new int;
    *isDone = 0;
	thread thread_( & PeerManager::receiveStatic, sockfd, Peer::CLIENT, hostname, port, isDone);
	thread_.detach();
	while(*isDone == 0){
		usleep(100);
	}
	//LogInfo("GHGHGHGHG");
    return true;
}

int PeerManager::send(int sockfd, string& str){
	signal(SIGPIPE, SIG_IGN);

	string fullStr = preStringBeacon + str + postStringBeacon;

	const char* buffer = fullStr.c_str();
	int n = write(sockfd, buffer, strlen(buffer));
    if (n <= 0) {
    	//LogError("ERROR writing %s to socket fd=%d", str.c_str(), sockfd);
    	return Error::ERROR_WRITING_TO_SOCKET;
    }
    return Error::NO_ERROR;
}


vector<ModuleUri> PeerManager::getListOfPeers(){
	vector<ModuleUri> res;
	mutex_peers.lock();
	for(auto& peer_ : peers)
		res.push_back(peer_.uri);
	mutex_peers.unlock();
	return res;
}

bool PeerManager::haveConnectionTo(ModuleUri& uri){
	mutex_peers.lock();
	for(unsigned int i = 0; i < peers.size(); i++){
		if(peers[i].uri.isTheSame(uri)){
			mutex_peers.unlock();
			return true;
		}
	}
	mutex_peers.unlock();
	return false;
}

void PeerManager::messageHandler(){
	//Change to condition variable
	while(true){
		pair<JsonObject, ModuleUri> thisJson;
		bool haveMessage = false;
		{
			unique_lock<mutex> lock_messages(singleton->mutex_messages);
			singleton->cv_messages.wait_for(lock_messages, chrono::milliseconds(1));
			if(singleton->messages.size() > 0){
				thisJson = singleton->messages.front();
				singleton->messages.pop_front();
				haveMessage = true;
			}
		}
		if( singleton->mJsonHandler != NULL && haveMessage){
			singleton->mJsonHandler(thisJson.first, thisJson.second);
		}
	}
}

int PeerManager::sendRegisterMessage(int sockfd){
	JsonObject msg;
	msg.PutString("message_type", REGISTER_MSG);
	msg.PutString("module_uri", ownUri->GetFullString().c_str());
	string str = msg.ToString();
	return send(sockfd, str);
}

bool PeerManager::sendJson(ModuleUri& toUri, const JsonObject& json){
	JsonObject jsonCpy = json;
	jsonCpy.PutString("from_module_uri", ownUri->GetFullString());
	jsonCpy.PutUInt("tx_time", current_date_time_msec() );
	unsigned int newUniqueId = (unsigned int) rand();
	jsonCpy.PutUInt("unique_message_id", newUniqueId);
	string str = jsonCpy.ToString();

	mutex_peers.lock();
	for(unsigned int i = 0; i < peers.size(); i++){
		if(peers[i].uri.isTheSame(toUri)){
			int err = send(peers[i].sockfd, str);
			mutex_peers.unlock();
			if( err == Error::NO_ERROR ) return true;
			else{
				LogError("Could not send message to uri = %s", toUri.GetFullString().c_str());
				return false;
			}
		}
	}
	mutex_peers.unlock();
	LogWarning("We still do not have a complete connection with %s", toUri.GetFullString().c_str());
	return false;
}

bool PeerManager::isWaitResponse(const ModuleUri& fromUri, const string& field, const string& val, JsonObject& replyJson, ModuleUri& replyUri){
	return ( replyUri.isTheSame(fromUri) &&
		replyJson.hasValue(field) &&
		replyJson.GetString(field) == val);
}

bool PeerManager::isWaitResponse(JsonObject& replyJson, ModuleUri& replyUri){
	return( amWaitingFor &&
		replyUri.isTheSame( amWaitingFor->fromUri ) &&
		replyJson.hasValue(amWaitingFor->field) &&
		replyJson.GetString(amWaitingFor->field) == amWaitingFor->val);
}

bool PeerManager::waitFor(const ModuleUri& fromUri, const string& field, const string& val, JsonObject& replyJson, int time0){
	unsigned long long timeInit = current_date_time_msec();
	{
		unique_lock<mutex> lock_messages(mutex_messages);
		if(amWaitingFor){
			LogError("can not perform two simultaneous waitFor operations!");
			return false;
		}

		amWaitingFor = new waitForStruct;
		if(amWaitingFor == NULL){
			LogError("amWaitingFor is NULL!");
			return false;
		}
		amWaitingFor->fromUri = fromUri;
		amWaitingFor->field = field;
		amWaitingFor->val = val;
		amWaitingFor->jsonReply = NULL;
	}

	while( true ){
		int timeDel = time0 - (int)(current_date_time_msec() - timeInit);
		if(timeDel <= 0) break;
		unique_lock<mutex> lock_messages(mutex_messages);
		cv_messages.wait_for(lock_messages, chrono::milliseconds( timeDel));
		if( amWaitingFor &&  amWaitingFor->jsonReply ){
			//replyJson = *amWaitingFor->jsonReply;
			//LogInfo("r1 %p , %p", amWaitingFor, amWaitingFor->jsonReply);
			amWaitingFor->jsonReply->copyTo( replyJson );
			delete amWaitingFor->jsonReply;
			amWaitingFor->jsonReply = NULL;
			delete amWaitingFor;
			amWaitingFor = NULL;
			return true;
		}
	}
	{
		unique_lock<mutex> lock_messages(mutex_messages);
		delete amWaitingFor->jsonReply;
		amWaitingFor->jsonReply = NULL;
		delete amWaitingFor;
		amWaitingFor = NULL;
	}
	LogWarning("Did not receive response in give time interval %d msec.", time0);
	return false;
}
