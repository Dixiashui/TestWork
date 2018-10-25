/* 
 * File:   LogToFile.h
 * Author: liul
 *
 * Created on 2011年7月18日, 下午11:45
 */

#ifndef LOGTOFILE_H
#define	LOGTOFILE_H

#include "stdio.h"
#include "unistd.h"
#include "string.h"

class LogToFile {
public:
    LogToFile();
    ~LogToFile();
    static FILE* OpenFile(char * fileName);
    static void writeStringln(char *fileName, char *prompt,char *buf);
    static void writeIntln(char *fileName,char *prompt,int i);
    static void writeCharln(char *fileName,char *prompt,char c);
    static void writeBinaryStreamln(char * fileName, unsigned char *buf, int size);
    static void writeBuffer(char *fileName, unsigned char *buf, int size); 
    static int getFileSize(char *fileName);
};



#endif	/* LOGTOFILE_H */

