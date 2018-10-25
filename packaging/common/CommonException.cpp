#include "common/CommonException.h"

/**
 * @brief CommonException constructor.
 *
 * @param Exception description.
 */
CommonException::CommonException(const std::string &exp)
{
    _exception = exp;
}

/**
 * @brief CommonException destructor.
 */
CommonException::~CommonException() throw ()
{
    // do nothing
}

/**
 * @brief Returns the exception description. 
 *
 * @returns The general cause of the current error.
 */
const char * CommonException::what() const throw()
{
    return _exception.c_str();
}

