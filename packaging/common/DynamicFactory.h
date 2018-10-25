/* 
 * File:   DynamicFactory.h
 * Author: qianl
 *
 * Created on October 24, 2013, 4:12 PM
 */

#ifndef DYNAMICFACTORY_H
#define	DYNAMICFACTORY_H

#include <string>
#include <map>

#define DECLARE_CLASS \
static DynamicClassRegister classRegister;

#define REGISTER_CLASS(className) \
DynamicClassRegister className::classRegister = DynamicClassRegister(#className, (createFunction)className::CreateObject);

#define CREATE_FUNCTION(className) \
static void* CreateObject() { \
    return new className(); \
}


typedef void* (*createFunction)();

class DynamicFactory {
public:
    void* getInstanceByClassName(std::string name);
    void registClass(std::string name, createFunction func);
    static DynamicFactory& getInstance();
private:
    std::map<std::string, createFunction> classMap;
    DynamicFactory();
    virtual ~DynamicFactory();
};

class DynamicClassRegister
{
public:
    DynamicClassRegister(std::string name, createFunction func)
    {
        DynamicFactory::getInstance().registClass(name, func);
    }
};

#endif	/* DYNAMICFACTORY_H */

