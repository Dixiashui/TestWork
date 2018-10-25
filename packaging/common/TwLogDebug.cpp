#include "TwLog.h"
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <string>

///#include <JsiTwConnection.h>

int TwLog::mLevel = LOG_TW_DBUG;
Mutex TwLog::mLevelMutex;

/**
 * @brief method to log a message
 * 
 * Send the log message to Pegasus or/and to Syslog, depends the environment variables.
 * It could be send with parameters as in printf.
 * Ex: log(LOG_TW_INFO , "Test message number:%i ", 4)
 * 
 * @param priority priority for log
 * @param message message to be loged
 */
void TwLog::log(int priority, const char *message, ...){

	va_list vargs;
	char msg[1000];

	if(priority > mLevel)
		return;

	va_start (vargs, message);
	
	//if there is % there is not easy to determine the length, I'm using +50 to avoid problems
	std::string msg_final;

	//It is necesary to use vsprintf to use all parameter comming from ...
	vsnprintf(msg, 1000, message, vargs);
	msg[999]='\0';

	//Include priority description on the message
	if(priority<=LOG_TW_ERRO){
		msg_final = "[ERRO] ";
	}else if( priority <= LOG_TW_WARN){
		msg_final = "[WARN] ";
	}else if( priority <= LOG_TW_INFO){
		msg_final = "[INFO] ";
	}else if( priority <= LOG_TW_TRAC){
		msg_final = "[TRAC] ";
	}else if( priority <= LOG_TW_DBUG){
		msg_final = "[DBUG] ";
	} 
	msg_final += msg;

	sendLogToSyslog(priority, msg_final.c_str());
///	sendLogToPegasus(priority, msg_final.c_str());

	va_end(vargs);

}

/**
 * @brief send log message to syslog
 */
 void TwLog::sendLogToSyslog(int priority, const char *msg){

	int sysLogPrio = 0;

	if(priority<=LOG_TW_ERRO){
		sysLogPrio = 3;
	}else if( priority <= LOG_TW_WARN){
		sysLogPrio = 4;
	}else if( priority <= LOG_TW_INFO){
		sysLogPrio = 5;
	}else if( priority <= LOG_TW_TRAC){
		sysLogPrio = 6;
	}else if( priority <= LOG_TW_DBUG){
		sysLogPrio = 7;
	}
 
	openlog("Testwork", LOG_CONS | LOG_PID, LOG_LOCAL0);
	syslog(sysLogPrio, msg);
	closelog();

}
/*
void TwLog::sendLogToPegasus(int severity, const char *msg)
{

    JsiTwConnection *con = JsiTwConnection::getInstance();
   
    char *msgbuf = (char*) malloc ( strlen(msg) * sizeof(char) + 2 );
    
    // first byte -> severity
    msgbuf[0] = (char) severity;
    
    // message
    strcpy (&msgbuf[1], msg);
    
    con->sendMessage(LOGMSG, msgbuf, strlen(msg)+1 );
    
    free(msgbuf); 

}
*/

void TwLog::setModuleName(std::string module){
    mModuleName = "<";
    mModuleName.append(module);
    mModuleName.append(">");
}

std::string TwLog::getModuleName(){
    return mModuleName;
}

/**
 * @brief constructor
 */
TwLog::TwLog(){
	
}

/**
 * @brief destructor
 */
TwLog::~TwLog(){
	
}
