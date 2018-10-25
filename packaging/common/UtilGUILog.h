#ifndef _UTILGUILOG_H
#define _UTILGUILOG_H

/**
 * @file UtilGUILog.h
 * @brief Defines the UtilGUILog class.
 */

#include "common/Util.h"
#include "common/TwLog.h"

#include <time.h>
#include <string>

/**
 * @brief UtilGUILog class.
 *
 */
class UtilGUILog
{
private:
    /**
     * @brief UtilGUILog contructor. UtilGUILog cannot be instanced.
     */
    UtilGUILog();

    /**
     * @brief UtilGUILog destructor. UtilGUILog cannot be deleted.
     */
    ~UtilGUILog();

public:
    static void logDbug(const std::string &s);
    static void logTrac(const std::string &s);
    static void logInfo(const std::string &s);
    static void logWarn(const std::string &s);
    static void logErro(const std::string &s);

private:
    static TwLog mTwLog;    ///< Testwork LOG.
};

#endif // ifndef _UTILGUILOG_H


