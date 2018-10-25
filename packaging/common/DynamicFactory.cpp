/* 
 * File:   DynamicFactory.cpp
 * Author: qianl
 * 
 * Created on October 24, 2013, 4:12 PM
 */

#include "DynamicFactory.h"

DynamicFactory& DynamicFactory::getInstance()
{
    static DynamicFactory thisInstance;
    return thisInstance;
}

void DynamicFactory::registClass(std::string name, createFunction func)
{
    classMap.insert(std::make_pair(name, func));
}

void* DynamicFactory::getInstanceByClassName(std::string name)
{
    if(classMap.find(name) == classMap.end())
        return NULL;
    else
        return classMap.find(name)->second();
}

DynamicFactory::DynamicFactory()
{
}

DynamicFactory::~DynamicFactory()
{
}

