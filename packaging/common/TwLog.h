#ifndef _TWLOG_H
#define _TWLOG_H

#define LOG_TW_DEV  2
#define LOG_TW_DBUG 90
#define LOG_TW_TRAC 20
#define LOG_TW_INFO 10
#define LOG_TW_WARN 8
#define LOG_TW_ERRO 4

#define LOG_NUM_OF_COUNTERS 5

#define LOG_COUNTER(counterNumber) log.counter[counterNumber]++
#define LOG_COUNT(counterNumber) log(LOG_TW_DEV, "DEVLOG %s COUNTER(%d):%d  " , getModuleName().c_str(), counterNumber, LOG_COUNTER(counterNumber));

#define LOG_DEV(string, args...) log.log(LOG_TW_DEV, "DEVLOG %s %s  " #string "              (file:%s, line:%d)" , log.getModuleName().c_str(), __FUNCTION__, ## args, __FILE__, __LINE__)

#define LOG_TRAC(string, args...) log.log(LOG_TW_TRAC, "%s %s  " #string , log.getModuleName().c_str(), __FUNCTION__, ## args); \
                                  log.log(LOG_TW_DBUG, "(file:%s, function:%s, line:%d)" , __FILE__, __FUNCTION__, __LINE__)
#define LOG_DBUG(string, args...) log.log(LOG_TW_DBUG, "%s %s  " #string , log.getModuleName().c_str(), __FUNCTION__, ## args); \
                                  log.log(LOG_TW_DBUG, "(file:%s, function:%s, line:%d)" , __FILE__, __FUNCTION__, __LINE__)
#define LOG_ERRO(string, args...) log.log(LOG_TW_ERRO, "%s %s  " #string , log.getModuleName().c_str(), __FUNCTION__, ## args); \
                                  log.log(LOG_TW_DBUG, "(file:%s, function:%s, line:%d)" , __FILE__, __FUNCTION__, __LINE__)
#define LOG_WARN(string, args...) log.log(LOG_TW_WARN, "%s %s  " #string , log.getModuleName().c_str(), __FUNCTION__, ## args); \
                                  log.log(LOG_TW_DBUG, "(file:%s, function:%s, line:%d)" , __FILE__, __FUNCTION__, __LINE__)
#define LOG_INFO(string, args...) log.log(LOG_TW_INFO, "%s %s  " #string , log.getModuleName().c_str(), __FUNCTION__, ## args); \
                                  log.log(LOG_TW_DBUG, "(file:%s, function:%s, line:%d)" , __FILE__, __FUNCTION__, __LINE__)

#define LOG_TRACE(string, args...) LOG_TRAC(string, ## args)
#define LOG_DEBUG(string, args...) LOG_DBUG(string, ## args)
#define LOG_ERROR(string, args...) LOG_ERRO(string, ## args)
#define LOG_WARNING(string, args...) LOG_WARN(string, ## args)
        
#include "common/Mutex.h"

/**
 * @brief This class implemets a log wrapper.
 *
 * This class is called by all Testwork project to log.<br>
 * It forward the log message to Pegasus and to Syslog.
 * 
 * @author Eduardo Schnell e Schuhli <eduardo.schnell@siemens.com>
 */
 class TwLog
{
public:
    TwLog();
    ~TwLog();

    void log(int priority, const char *message, ...);
	
public:
    void setModuleName(std::string module);
    std::string getModuleName();
    static int mLevel;
    static Mutex mLevelMutex;
    unsigned int counter[LOG_NUM_OF_COUNTERS];

private:
    void sendLogToPegasus(int severity, const char *msg);
    void sendLogToSyslog(int severity, const char *msg);
    
    std::string mModuleName;
};


#endif

