/*
 * Bash.h
 *
 *  Created on: Jan 25, 2016
 *      Author: mahdy
 */

#ifndef SRC_UTILS_BASH_H_
#define SRC_UTILS_BASH_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <utils/Logging.h>

class Bash{
private:
	static Bash* mSingleton;
	Bash();
public:
	static Bash* GetInstance();

	static int executeBlockingSystemCommand(std::string command);
	static int executeBlockingSystemCommand(const char* command);

	static string executeBlockingSystemCommandWithResponse(const string& cmd);

	~Bash();
};



#endif /* SRC_UTILS_BASH_H_ */
