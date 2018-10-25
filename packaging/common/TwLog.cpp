#include "TwLog.h"
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <string>

#include <JsiTwConnection.h>

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
void TwLog::log(int priority, const char *message, ...) {

    va_list vargs;
    char msg[1000];

    //could be PEGASUS, SYSLOG or BOTH
    char* envVarLogWayChar = getenv("TW_LOG_PATH");
    std::string envVarLogWay = envVarLogWayChar ? envVarLogWayChar : "";
    //this if exist to cut some logs to increase performance
    if (priority > mLevel)
        return;

    va_start(vargs, message);

    //It is necesary to use vsprintf to use all parameter comming from ...
    vsnprintf(msg, 1000, message, vargs);
    msg[999] = '\0';

    /*	if(envVarLogWay == ""){
                    sendLogToSyslog(priority, msg);
                    sendLogToPegasus(priority, msg);
            }else if(envVarLogWay == "PEGASUS"){
                    sendLogToPegasus(priority, msg);		
            }else if(envVarLogWay == "SYSLOG"){
                    sendLogToSyslog(priority, msg);		
            }else{
                    sendLogToSyslog(priority, msg);
                    sendLogToPegasus(priority, msg);
            }
     */
    sendLogToPegasus(priority, msg);

    va_end(vargs);

}

/**
 * @brief send log message to syslog
 */
void TwLog::sendLogToSyslog(int priority, const char *msg) {

    int sysLogPrio = 0;

    if (priority <= LOG_TW_ERRO) {
        sysLogPrio = 3;
    }else if (priority <= LOG_TW_WARN) {
        sysLogPrio = 4;
    }else if (priority <= LOG_TW_INFO) {
        sysLogPrio = 5;
    }else if (priority <= LOG_TW_TRAC) {
        sysLogPrio = 6;
    }else if (priority <= LOG_TW_DBUG) {
        sysLogPrio = 7;
    }

    openlog("Testwork", LOG_CONS | LOG_PID, LOG_LOCAL0);
    syslog(sysLogPrio, msg);
    closelog();

    /*	
            syslog seems to be slow, even open and write a file every log is faster than syslog,
            it should be re thinked in Log new generation
        std::ofstream os;
            os.open("/tmp/teste.log", std::ios_base::app);
            os << msg << std::endl;
            os.close();
     */
}

/**
 * @brief send log message to pegasus
 */
void TwLog::sendLogToPegasus(int severity, const char *msg) {
    JsiTwConnection *con = JsiTwConnection::getInstance();
    if (con->getLogEnabled()) {

        char *msgbuf = (char*) malloc(strlen(msg) * sizeof (char) + 2);

        // first byte -> severity
        msgbuf[0] = (char) severity;

        // message
        strcpy(&msgbuf[1], msg);

        con->sendMessage(LOGMSG, msgbuf, strlen(msg) + 1);

        free(msgbuf);
    }
}

void TwLog::setModuleName(std::string module) {
    mModuleName = "<";
    mModuleName.append(module);
    mModuleName.append(">");
}

std::string TwLog::getModuleName() {
    return mModuleName;
}

/**
 * @brief constructor
 */
TwLog::TwLog() {
    for (int i = 0; i < LOG_NUM_OF_COUNTERS; i++) {
        this->counter[i] = 0;
    }
}

/**
 * @brief destructor
 */
TwLog::~TwLog() {
}
