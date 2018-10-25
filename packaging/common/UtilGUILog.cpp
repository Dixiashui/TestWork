#include "TwLog.h"
#include "UtilGUILog.h"

TwLog UtilGUILog::mTwLog; 

/**
 * @brief Log debug.
 *
 * @param s Text to be logged.
 */
void UtilGUILog::logDbug(const std::string &s)
{
    mTwLog.log(LOG_TW_DBUG, "%s", s.c_str());
}
    
/**
 * @brief Log trace.
 *
 * @param s Text to be logged.
 */
void UtilGUILog::logTrac(const std::string &s)
{
    mTwLog.log(LOG_TW_TRAC, "%s", s.c_str());
}

/**
 * @brief Log information.
 *
 * @param s Text to be logged.
 */
void UtilGUILog::logInfo(const std::string &s)
{
    mTwLog.log(LOG_TW_INFO, "%s", s.c_str());
}

/**
 * @brief Log warning.
 *
 * @param s Text to be logged.
 */
void UtilGUILog::logWarn(const std::string &s)
{
    mTwLog.log(LOG_TW_WARN, "%s", s.c_str());
}

/**
 * @brief Log error.
 *
 * @param s Text to be logged.
 */
void UtilGUILog::logErro(const std::string &s)
{
    mTwLog.log(LOG_TW_ERRO, "%s", s.c_str());
}


