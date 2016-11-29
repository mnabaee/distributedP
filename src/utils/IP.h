/*
 * IP.h
 *
 *  Created on: Jan 24, 2016
 *      Author: mahdy
 */

#ifndef SRC_UTILS_IP_H_
#define SRC_UTILS_IP_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

class IP{
private:

public:
	static std::string GetOwnIP();
	static std::string GetOwnHostName();
};



#endif /* SRC_UTILS_IP_H_ */
