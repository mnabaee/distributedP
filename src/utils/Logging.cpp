#include "Logging.h"
#include <types/ModuleUri.h>

Logging* Logging:: m_pThis = NULL;

Logging::Logging()
{
	//mLogLevel = Logging::ERROR;
	mLogLevel = Logging::INFO;
	logFileName = "";
}

Logging::~Logging(){
	m_Logfile.close();
}

Logging* Logging::getInstance(){
    if(m_pThis == NULL){
        m_pThis = new Logging();
        if(m_pThis->logFileName != "")
        	m_pThis->m_Logfile.open(m_pThis->logFileName.c_str(), ios::out | ios::app );
    }
    return m_pThis;
}

std::string Logging::GetStackTrace(unsigned int max_frames){
	std::string res = "";

	// storage array for stack trace address data
	   void* addrlist[max_frames+1];

	   // retrieve current stack addresses
	   int addrlen = backtrace( addrlist, sizeof( addrlist ) / sizeof( void* ));

	   if ( addrlen == 0 )
	   {
		   return res;
	   }

	   // create readable strings to each frame.
	   char** symbollist = backtrace_symbols( addrlist, addrlen );

	   // print the stack trace.
	   for ( int i = 10; i < addrlen; i++ ){
		   std::string thisRes(symbollist[i]);
		   res += thisRes + "\n";
	   }
	   free(symbollist);

	return res;
}

void Logging::SetLogFileName(string filename){
	logFileName = filename;
    if(logFileName != "")
    	m_Logfile.open(logFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::app );
}


void Logging::Log( int loglevel, const char* filename, int lineNum, const char * format, ... )
{
	if(!canLog(loglevel)){
		return;
	}

    char* sMessage = NULL;

    va_list args;
    va_start (args, format);

    char prefixStr[128];

    sprintf (prefixStr, "[%s] %s, %s(%d), Time(%s), %s:%d", GetLogLevelStr(loglevel).c_str(), IP::GetOwnIP().c_str(), ModuleUri::GetOwnExeName().c_str(), getpid(), current_date_time_str().c_str(), filename, lineNum);

    vasprintf(&sMessage, format, args);

	if(loglevel <= Logging::ERROR){
		string stackTrace = GetStackTrace();
		printf("%s :: %s \n %s \n",prefixStr, sMessage, stackTrace.c_str());
		if(logFileName != ""){
			m_Logfile << prefixStr << " :: " << sMessage << "\n" << stackTrace.c_str() << "\n";
			m_Logfile.flush();
		}

	}else{
		printf("%s :: %s \n",prefixStr, sMessage);
		if(logFileName != ""){
			m_Logfile << prefixStr << " :: " << sMessage << "\n" ;
			m_Logfile.flush();
		}
	}
	fflush(NULL);
	free(sMessage);
    va_end (args);
}


Logging& Logging::operator<<(const string& sMessage )
{
    m_Logfile <<"\n"<<current_date_time_str()<<":\t";
    m_Logfile << sMessage << endl;
    return *this;
}

bool Logging::canLog(int loglevel){
	if( loglevel <= this->mLogLevel	){
		return true;
	}else{
		return false;
	}

}

std::string Logging::GetLogLevelStr(int level){
	if(level == Logging::INFO){
		return "INFO";
	}else if(level == Logging::WARNING){
		return ANSI_COLOR_YELLOW "WARNING" ANSI_COLOR_RESET;
	}else if(level == Logging::ERROR){
		return ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET;
	}else if(level == Logging::FATAL){
		return ANSI_COLOR_CYAN "FATAL" ANSI_COLOR_RESET;
	}else{
		return "";
	}
}
