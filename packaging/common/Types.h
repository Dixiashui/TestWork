#ifndef _TYPES_H
#define _TYPES_H

/**
 * @file Types.h
 *
 * @brief Defines common user defined types.
 */

#include <map>
#include <vector>

typedef std::map<int, std::string> MapIntToString;
typedef std::map<int, std::string>::iterator IMapIntToString;

typedef std::vector<std::string> VectorString;

#endif // ifndef _TYPES_H

