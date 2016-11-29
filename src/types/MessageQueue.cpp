/*
 * MessageQueue.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: mahdy
 */

#include "../types/MessageQueue.h"

MessageQueue::MessageQueue(){
}

MessageQueue::~MessageQueue(){

}

unsigned int MessageQueue::pushToEnd(JsonObject& newJO){
	newJO.PutInt("mq_time", current_date_time_msec());

	pthread_mutex_lock(&p_mutex);
		mq.push_back(newJO);
		int size = mq.size();
	pthread_mutex_unlock(&p_mutex);

	return size;
}

std::string MessageQueue::ToString(){
	std::string res = "";
	pthread_mutex_lock(&p_mutex);
		for(std::list<JsonObject>::iterator it = mq.begin(); it != mq.end(); it++){
			res += it->ToString() + "\n";
		}
	pthread_mutex_unlock(&p_mutex);
	return res;
}

int MessageQueue::popFromBegin(JsonObject& popJO){
	pthread_mutex_lock(&p_mutex);
		if(mq.size() < 1){
			return -1;
		}
		mq.begin()->copyTo(popJO);
		mq.erase(mq.begin());
		int size = mq.size();
	pthread_mutex_unlock(&p_mutex);
	return size;
}
