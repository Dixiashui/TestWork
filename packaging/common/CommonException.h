#ifndef _COMMON_EXCEPTION_H_
#define _COMMON_EXCEPTION_H_

#include <exception>
#include <string>

/**
 * @file CommonException.h
 * @brief Defines the CommonException class.
 */

/**
 * @brief This is the class used to throw exceptions by common modules.
 *
 * @author Alexandre Maia Godoi <alexandre.godoi@nsn.com>
 */
class CommonException : public std::exception
{
public:
    CommonException(const std::string &exp);
    virtual ~CommonException() throw ();

    virtual const char * what() const throw();

private:
    std::string _exception;
};

#endif // _COMMON_EXCEPTION_H_

