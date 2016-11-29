/*
 * MessageQueue.h
 *
 *  Created on: Jan 20, 2016
 *      Author: mahdy
 */

#ifndef SRC_TYPES_MESSAGEQUEUE_H_
#define SRC_TYPES_MESSAGEQUEUE_H_

#include <string>
#include <utils/Logging.h>
#include <utils/DateTime.h>
#include <types/JsonObject.h>
#include <list>
#include <pthread.h>

class MessageQueue{
private:
	std::list<JsonObject> mq;
	pthread_mutex_t p_mutex;
public:
	MessageQueue();
	~MessageQueue();

	unsigned int  pushToEnd(JsonObject& newJO);
	int  popFromBegin(JsonObject& popJO);

	std::string ToString();

	unsigned int GetSize(){return mq.size();};

};

#endif /* SRC_TYPES_MESSAGEQUEUE_H_ */
