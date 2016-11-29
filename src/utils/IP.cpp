/*
 * IP.cpp
 *
 *  Created on: Jan 24, 2016
 *      Author: mahdy
 */

#include "IP.h"
#include <unistd.h>

std::string IP::GetOwnIP(){
	return GetOwnHostName();

	int fd;
	struct ifreq ifr;

	char iface[] = "eth0";

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;

	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	std::string res(inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));

	return res;
}

std::string IP::GetOwnHostName(){
	size_t maxLen = 1024;
	char hostname[maxLen];
	gethostname(hostname, maxLen);
	return std::string(hostname);
}
