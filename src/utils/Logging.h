#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include <thread>
#include <sys/types.h>
#include <unistd.h>
#include "DateTime.h"
#include "IP.h"
#include <execinfo.h>


using namespace std;

#define Logger(loglevel, format, ...) Logging::getInstance()->Log(loglevel, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define LogInfo(format, ...) Logging::getInstance()->Log(Logging::INFO, __FILE__, __LINE__, format, ##__VA_ARGS__);
#define LogWarning(format, ...) Logging::getInstance()->Log(Logging::WARNING, __FILE__, __LINE__, format, ##__VA_ARGS__);
#define LogError(format, ...) Logging::getInstance()->Log(Logging::ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__);
#define LogFatal(format, ...) Logging::getInstance()->Log(Logging::FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

class Logging
{
public:
	~Logging();
	enum LogLevels{
		INFO = -100,
		WARNING = -200,
		ERROR = -300,
		FATAL = -400
	};

    void Log(int loglevel, const char* filename, int lineNum, const char * format, ... );
    Logging& operator<<(const string& sMessage );
    static Logging* getInstance();

	std::string GetStackTrace(unsigned int max_frames = 100);

	void SetLogLevel(int loglevel){this->mLogLevel = loglevel;};

	void SetLogFileName(string filename);

private:
    Logging();
    Logging( const Logging&){};             // copy constructor is private
    Logging& operator=(const Logging& ){ return *this;};  // assignment operator is private
    static Logging* m_pThis;

    int mLogLevel;
    string logFileName;
    ofstream m_Logfile;

    bool canLog(int loglevel);
    std::string GetLogLevelStr(int level);

};

#endif
